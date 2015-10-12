#ifndef CRYPTO_H
#define CRYPTO_H

#include "shared/macros.h"
#include "shared/util.h"
#include "shared/AES_RNG.h"
#include <sys/socket.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#if defined BANK
#include "bank/server.h" /* need db access if in bank binary */
#include "bank/db.h"
#endif


#define TAG_SIZE 16 /* CryptoPP::GCM::GCM_Base.DigestSize() */
#define KEY_SIZE CryptoPP::AES::MAX_KEYLENGTH /* 32 bytes = 256 bits */
#define IV_SIZE 12 /* 96 bits as recommended by AES GCM spec */

int encrypt(char* plain, int plen, char* cipher, unsigned char* key, 
	    unsigned char* iv);

int decrypt(char* cipher, int clen, char* plain, unsigned char* key, 
	    unsigned char* iv);

ssize_t secure_send(int sockfd, const void* buf, size_t len,
		    unsigned char* key, unsigned char* iv);

ssize_t secure_recv(int sockfd, void* buf, unsigned char* key,
		    unsigned char* iv);

AES_RNG* init_iv_gen(unsigned char* iv);

int get_next_iv(AES_RNG* prng, unsigned char* iv);

void free_iv_gen(AES_RNG* prng);

#endif
