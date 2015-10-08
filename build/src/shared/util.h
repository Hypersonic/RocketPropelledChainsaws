#ifndef UTIL_H
#define UTIL_H

#include <string>
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

int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt,
    EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);

void hmac_init(HMAC_CTX *ctx, const char* key, size_t len);
void hmac_update(HMAC_CTX *ctx, const char* bytes, size_t len);
void hmac_digest(HMAC_CTX *ctx, char* digest);

int is_valid_name(char *name);
int is_valid_filename(char *filename);

void print_escaped_string(char *str);
int random_bytes(char* buf, size_t len);

unsigned get_file_size(const char * file_name);
int read_from_file(char *dst, unsigned read_size, char *file_name);
int write_to_file(char *src, unsigned write_size, char *file_name);

#endif
