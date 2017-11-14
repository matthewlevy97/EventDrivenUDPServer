#include <netinet/in.h>

#ifndef UDPSERVER_H
#define UDPSERVER_H

#define DEBUG_MODE

// CONSTANTS
#define MAX_BUFFER_LENGTH   100

// ERROR CODES
#define SOCKET_CREATE_ERROR -1
#define SOCKET_BIND_ERROR   -2
#define SOCKET_NAME_ERROR   -3
#define FCNTL_SETOWN_ERROR  -4
#define FCNTL_FASYNC_ERROR  -5
#define SETUP_FAILURE       -6
#define SUCCESS 0

int bindServer(int port);
void registerUDPCallback(void(*func)(void *, int, struct sockaddr_in));

#endif
