#ifndef _SSL_HELPER_H
#define _SSL_HELPER_H

#include <openssl/ssl.h>
#include <openssl/err.h>

/* Don't forget to compile with -lcrypto */

void init_openssl();

void cleanup_openssl();

SSL_CTX *create_context();

void configure_context(
    SSL_CTX *ctx, 
    char *cert_path, 
    char *key_path
);

#endif