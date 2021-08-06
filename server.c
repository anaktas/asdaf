#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "socket_helper.h"
#include "ssl_helper.h"
#include "server.h"

void listen_and_serve(int port) {
    printf("Starting server...\n");
    int socket;
    SSL_CTX *context;

    init_openssl();
    context = create_context();

    configure_context(context, "server.cert", "server.key");

    printf("Creating socket...\n");
    socket = create_socket(port);

    /* Handle connections */
    while(1) {
        printf("\nHandling connections...\n");
        struct sockaddr_in address;
        uint len = sizeof(address);
        SSL *ssl;
        //const char default_reply[] = "HTTP/1.1 200 OK\r\n\r\nGreetings!\r\n";

        int client = accept(socket, (struct sockaddr*)&address, &len);
        if (client < 0) {
            perror("Error: Unable to accept.");
            return;
        }

        ssl = SSL_new(context);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) > 0) {
            Request request;
            Header request_header[17] = { {"\0", "\0"} };
            char *buffer = (char*) malloc(65535);
            int received;
            char *method, *uri, *query_paramaters, *protocol, *payload;
            int payload_size;

            received = SSL_read(ssl, buffer, 65535);

            printf("Decoded packet: %s\n", buffer);
            printf("Received bytes: %d\n", received);

            if (received == -1 || received == 0) {
                int error = SSL_get_error(ssl, received);
                if (
                    error == SSL_ERROR_ZERO_RETURN 
                    || error == SSL_ERROR_NONE 
                    || error == SSL_ERROR_WANT_READ
                ) {
                    perror("Error: Error while reading request\n");
                }
            } else {
                buffer[received] = '\0';

                method = strtok(buffer,  " \t\r\n");
                uri = strtok(NULL, " \t");
                protocol = strtok(NULL, " \t\r\n");

                fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

                if (query_paramaters = strchr(uri, '?')) {
                    *query_paramaters++ = '\0'; //split URI
                } else {
                    query_paramaters = uri - 1; //use an empty string
                }

                Header *header = request_header;
                char *temp, *last_header;

                while(header < request_header + 16) {
                    char *key,*value;
                    key = strtok(NULL, "\r\n: \t"); if (!key) break;
                    value = strtok(NULL, "\r\n"); while(*value && *value == ' ') value++;
                    header->key = key;
                    header->value = value;
                    header++;
                    fprintf(stderr, "[H] %s: %s\n", key, value);
                    temp = value + 1 + strlen(value);
                    if (temp[1] == '\r' && temp[2] == '\n') break;
                }

                temp++; // now the *t shall be the beginning of user payload
                last_header = request_a_header("Content-Length", request_header); // and the related header if there is  
                payload = temp;
                payload_size = last_header ? atol(last_header) : (received - (temp - buffer));

                printf("Request method: %s\n", method);
                printf("Request URI: %s\n", uri);
                printf("Request protocol: %s\n", protocol);
                printf("Request query parameters: %s\n", query_paramaters);
                printf("Request payload: %s\n", payload);
                printf("Request payload size: %d\n", payload_size);

                request.method = method;
                request.uri = uri;
                request.query_paramaters = query_paramaters;
                request.protocol = protocol;
                request.payload = payload;
                request.payload_size;
            }

            //printf("Replying: %s\n", default_reply);
            //SSL_write(ssl, default_reply, strlen(default_reply));
            printf("Routing the request...\n");
            router(ssl, &request);

            free(buffer);

            goto close_ssl;
        }

        ERR_print_errors_fp(stderr);

close_ssl:
        printf("Shutdown ssl...\n");
        SSL_shutdown(ssl);
        printf("Free ssl...\n");
        SSL_free(ssl);
        printf("Closing client socket...\n");
        close(client);
        printf("...Done\n");
    }

    close(socket);
    SSL_CTX_free(context);
    cleanup_openssl();
}

char *request_a_header(const char *key, Header *header) {
    Header *h = header;

    while(h->key) {
        if (strcmp(h->key, key) == 0) return h->value;
        h++;
    }

    return NULL;
}