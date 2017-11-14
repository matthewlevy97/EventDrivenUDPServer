CC=gcc
CFLAGS=-Wall

all: udpserver

udpserver: udpserver.o example.o
	$(CC) udpserver.o example.o -o udpserver

example.o: example.c
	$(CC) $(CFLAGS) -c example.c

udpserver.o: udpserver.c udpserver.h
	$(CC) $(CFLAGS) -c udpserver.c

clean:
	rm -rf *.o udpserver
