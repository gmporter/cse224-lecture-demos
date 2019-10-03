#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Practical.h"

using namespace std;

/* Given a sockaddr struct, return it as a string (from D&C book) */
char * get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

/* a.out host port */
int main(int argc, char *argv[]) {
	if (argc != 3) {
		DieWithUserMessage("Parameter(s)", "host port");
	}

	const char * host = argv[1];
	const char * port = argv[2];

	struct addrinfo hints, *servinfo, *p;
	int rv, sock;

	fprintf(stderr, "Connecting to %s:%s\n", host, port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;     // either IPv6 or IPV4 OK
	hints.ai_socktype = SOCK_STREAM; // use tcp

	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		DieWithUserMessage("getaddrinfo", gai_strerror(rv));
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sock = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			DieWithUserMessage("socket", "Couldn't connect to address");
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			DieWithUserMessage("socket", "can't connect");
		}

		break;
	}

	if (p == NULL) {
		DieWithUserMessage("socket", "Didn't find an address to connect to");
	}
	freeaddrinfo(servinfo);

	char addrbuf[128];
	get_ip_str(p->ai_addr, addrbuf, 128);
	fprintf(stderr, "Connected to %s!\n", addrbuf);

	const char * message = "Hi from CSE 224!\n";
	int ret = send(sock, message, strlen(message), 0);
	if (ret != strlen(message)) {
		DieWithSystemMessage("send() failed");
	}

	// Receive the response
	ssize_t numBytes;
	do {
		char buffer[BUFSIZE]; // I/O buffer
		numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
		if (numBytes < 0)
			DieWithSystemMessage("recv() failed");
		else if (numBytes == 0)
			break;
		buffer[numBytes] = '\0';    // Terminate the string!
		fputs(buffer, stdout);      // Print the echo buffer
	} while (numBytes > 0);

	fputc('\n', stdout); // Print a final linefeed

	close(sock);
	fprintf(stderr, "Closed the socket!\n");

	exit(0);
}

char * get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
	switch(sa->sa_family) {
		case AF_INET:
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
				s, maxlen);
				break;

		case AF_INET6:
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
				s, maxlen);
				break;

		default:
			strncpy(s, "Unknown AF", maxlen);
			return NULL;
	}

	return s;
}
