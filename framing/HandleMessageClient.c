#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/types.h>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <string>
#include <string.h>
#include <iostream>

#include "Practical.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

using namespace std;

void HandleMessageClient(int clntSocket)
{
	string msgbuffer;
	char recvbuf[RCVBUFSIZE];

	while(true) {
		memset(recvbuf, 0, RCVBUFSIZE); // clear out the receive buffer
		int numRecvBytes = recv(clntSocket, recvbuf, RCVBUFSIZE, 0);

		if (numRecvBytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				fprintf(stderr, "Client timed out, closing connection\n");
				close(clntSocket);
				return;
			}

			perror("recv");
			close(clntSocket);
			return;
		}

		if (numRecvBytes == 0) {
			fprintf(stderr, "remote end closed the connection\n");
			close(clntSocket);
			return;
		}

		msgbuffer = msgbuffer + string(recvbuf, numRecvBytes);

		cout << "Message buffer is: " << msgbuffer.c_str() << endl;

		size_t delimoffset = msgbuffer.find("==");
		while (delimoffset != string::npos) {
			string msg = msgbuffer.substr(0, delimoffset);
			msgbuffer = msgbuffer.substr(delimoffset+2, string::npos);
			cout << "Message received: " << msg << endl;
			delimoffset = msgbuffer.find("==");
		}
	}
}
