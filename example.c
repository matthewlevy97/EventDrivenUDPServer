#include <stdio.h>
#include <stdlib.h>

#include "udpserver.h"

void demo(void * buffer, int bufferLength, struct sockaddr_in their_addr) {
	printf("CALL ME: %s\n", (char*)buffer);
	free(buffer);
}

int main() {
	registerUDPCallback(demo);
	int setup = bindServer(8080);
	printf("Bind Server Setup Value: %d\n", setup);
}
