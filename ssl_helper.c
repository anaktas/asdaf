#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "ssl_helper.h"

/**
 * Initializes the OpenSSL.
 */
void init_openssl() { 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

/**
 * Clean up the openssl.
 */
void cleanup_openssl() {
    EVP_cleanup();
}

/**
 * Creates an SSL context.
 * 
 * @return {@link SSL_CTX}
 */
SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	    perror("Unable to create SSL context.");
	    ERR_print_errors_fp(stderr);
	    exit(EXIT_FAILURE);
    }

    return ctx;
}

/**
 * Configures a given SSL_CTX pointer.
 * 
 * @param ctx an SSL_CTX pointer
 * @param cert_path the given certificate file path
 * @param key_path the given key file path
 */
void configure_context(
    SSL_CTX *ctx, 
    char *cert_path, 
    char *key_path
) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, cert_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	    exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	    exit(EXIT_FAILURE);
    }
}