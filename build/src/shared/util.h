#ifndef UTIL_H
#define UTIL_H

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "shared/macros.h"
#include "shared/transfer.h"

int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt,
    EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);

void hmac_init(HMAC_CTX *ctx, const char* key, size_t len);
void hmac_update(HMAC_CTX *ctx, const char* bytes, size_t len);
void hmac_digest(HMAC_CTX *ctx, char* digest);

UNUSED int random_bytes(char* buf, size_t len);

#endif
