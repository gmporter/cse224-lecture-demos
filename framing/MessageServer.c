#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "Practical.h"

void HandleMessageClient(int clntSocket);   /* TCP client handling function */

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32   /* Size of receive buffer */

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
	struct addrinfo hints, *servinfo, *p;
	int rv;
    struct sockaddr_in msgClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // Prefer IPv6, but allow IPv4
	hints.ai_socktype = SOCK_STREAM; // use TCP
	hints.ai_flags = AI_PASSIVE; // we're going to be a server

	if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((servSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("socket");
			continue;
		}

		if (bind(servSock, p->ai_addr, p->ai_addrlen) == -1) {
			close(servSock);
			perror("bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "failed to bind to a socket\n");
		exit(2);
	}

	freeaddrinfo(servinfo);

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(msgClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &msgClntAddr, 
                               &clntLen)) < 0)
            DieWithSystemMessage("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(msgClntAddr.sin_addr));

        HandleMessageClient(clntSock);
    }
    /* NOT REACHED */
}
