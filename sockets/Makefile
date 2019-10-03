CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic -Wno-sign-compare

all: client showip

showip: showip.c
	$(CC) $(CFLAGS) -o showip showip.c

client: TCPClient.c DieWithMessage.c
	$(CC) $(CFLAGS) -o client TCPClient.c DieWithMessage.c

clean:
	rm -rf client showip
