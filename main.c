#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include "server.h"

int main(int argc, char **argv) {
    listen_and_serve(5566);

    exit(EXIT_SUCCESS);
}

void router(SSL *ssl, Request *request) {
    ROUTE_START()

    ROUTE("GET", "/") {
        printf("Handling the GET request in root...\n");
        char response[65535];
        strcpy(response, "HTTP/1.1 200 OK\r\n\r\n");
        char *payload = "Greetings!\r\n";
        strcat(response, payload);

        SSL_write(ssl, response, strlen(response));
    }

    ROUTE("POST", "/") {
        printf("Handling the POST request in root...\n");
        char response[65535];
        strcpy(response, "HTTP/1.1 200 OK\r\n\r\n");
        char *payload = "Greetings!\r\n";
        strcat(response, payload);

        SSL_write(ssl, response, strlen(response));
    }
  
    ROUTE_END()
}