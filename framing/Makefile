CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic -Wno-sign-compare

all: server

server: MessageServer.c DieWithMessage.c HandleMessageClient.c
	$(CC) $(CFLAGS) -o server MessageServer.c DieWithMessage.c HandleMessageClient.c

clean:
	rm -rf server
