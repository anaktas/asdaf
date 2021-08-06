#ifndef _SERVER_H
#define _SERVER_H

#include <openssl/ssl.h>

typedef struct {
    char *method;
    char *uri;
    char *query_paramaters;
    char *protocol;
    char *payload;
    int payload_size;
} Request;

typedef struct {
    char *key;
    char *value;
} Header;


void listen_and_serve(int port);
char *request_a_header(const char *key, Header *header);

void router(SSL *ssl, Request *request);

#define ROUTE_START()       if (0) {
#define ROUTE(METHOD,URI)   } else if (strcmp(URI, request->uri) == 0 && strcmp(METHOD, request->method) == 0) {
#define ROUTE_GET(URI)      ROUTE("GET", URI) 
#define ROUTE_POST(URI)     ROUTE("POST", URI) 
#define ROUTE_END()         } else {\
                                SSL_write(ssl, "HTTP/1.1 500 Not Handled\r\n\r\n", strlen("HTTP/1.1 500 Not Handled\r\n\r\n")); \
                            }

#endif