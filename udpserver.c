#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "udpserver.h"

#ifdef DEBUG_MODE
#include <stdio.h>
#endif

// Setup the socket
int sock;

// Setup the default callback handler
void defaultCallback(void * buffer, int bufferLength, struct sockaddr_in their_addr) {
	write(1, "NEW DATAGRAM PACKET\n", 20);
	free(buffer);
}
void(*callback)(void *, int, struct sockaddr_in) = defaultCallback;

// IO Signal Handler
void io_handler(int signal) {
	char * buf = (char*)calloc(MAX_BUFFER_LENGTH, 1);
	
	int len;
	unsigned int addr_len;
	struct sockaddr_in their_addr;
	
	// Read in datagram
	if((len = recvfrom(sock, buf, MAX_BUFFER_LENGTH, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
	#ifdef DEBUG_MODE
		perror("io_handler recvfrom");
	#endif
		return;
	}
	
	callback(buf, len, their_addr);
}

int startupServer(int port) {
	unsigned int length;
	struct sockaddr_in server;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
	#ifdef DEBUG_MODE
		perror("opening datagram socket");
	#endif
		return SOCKET_CREATE_ERROR;
	}
	
	// Setup structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
	#ifdef DEBUG_MODE
		perror("binding datagram socket");
	#endif
		return SOCKET_BIND_ERROR;
	}
	
	length = sizeof(server);
	if(getsockname(sock, (struct sockaddr *)&server, &length) < 0) {
	#ifdef DEBUG_MODE
		perror("getting socket name");
	#endif
		return SOCKET_NAME_ERROR;
	}
	
	#ifdef DEBUG_MODE
	printf("Starting UDP Server on port: %d\n", port);
	#endif
	
	// Setup callback function
	signal(SIGIO, io_handler);
	
	if(fcntl(sock, F_SETOWN, getpid()) < 0) {
	#ifdef DEBUG_MODE
		perror("fcntl F_SETOWN");
	#endif
		return FCNTL_SETOWN_ERROR;
	}
	
	if(fcntl(sock, F_SETFL, FASYNC) < 0) {
	#ifdef DEBUG_MODE
		perror("fcntl F_SETFL, FASYNC");
	#endif
		return FCNTL_FASYNC_ERROR;
	}
	
	return SUCCESS;
}

/**
Function Paramter:
func(void * buffer, int bufferLength, struct sockaddr_in their_address)
*/
void registerUDPCallback(void(*func)(void *, int, struct sockaddr_in)) {
	callback = func;
}

int bindServer(int port) {
	// Create pipe for communication
	int p[2];
	if(pipe(p)) {
	#ifdef DEBUG_MODE
		perror("pipe() failed");
	#endif
		return SETUP_FAILURE;
	}
	
	// Daemonize server
	if(!fork()) {
		// This is the child
		close(p[0]); // Close the reading end
		
		// Get the return value for setting up the server
		int ret = startupServer(port);
		
		// Convert the return value into a string
		char buf[10];
		snprintf(buf, 9, "%d", ret);
		
		// Write return value to the buffer
		write(p[0], buf, 10);
		
		// Loop forever
		for(;;);
		
		// Never should reach here
		exit(0);
	}
	
	// This is the parent
	close(p[1]); // Close the writing end
	
	char buf[10];
	if(read(p[0], buf, 9) <= 0) {
	#ifdef DEBUG_MODE
		perror("reading startupServe() return value failed");
	#endif
		return SETUP_FAILURE;
	}
	
	int ret;
	sscanf(buf, "%d", &ret);
	
	return ret;
}
