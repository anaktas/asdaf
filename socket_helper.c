#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socket_helper.h"

/**
 * Creates a socket and returns it's file
 * descriptor.
 * 
 * @param port the port number of the socket
 * @return the file descriptor of the socket
 */
int create_socket(int port) {
    int file_descriptor;
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (file_descriptor < 0) {
	    perror("Unable to create socket.");
	    exit(EXIT_FAILURE);
    }

    if (bind(file_descriptor, (struct sockaddr*) &address, sizeof(address)) < 0) {
	    perror("Unable to bind.");
	    exit(EXIT_FAILURE);
    }

    // TODO: Chenge the second attribute with the connection amount parameter
    // given by the user.
    if (listen(file_descriptor, 1) < 0) {
	    perror("Unable to prepare for socket acceptances.");
	    exit(EXIT_FAILURE);
    }

    return file_descriptor;
}