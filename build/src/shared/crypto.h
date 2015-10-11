#ifndef CRYPTO_H
#define CRYPTO_H

#include "shared/macros.h"
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>

#define TAG_SIZE 16 /* CryptoPP::GCM::GCM_Base.DigestSize() */
#define KEY_SIZE CryptoPP::AES::MAX_KEYLENGTH /* 32 bytes = 256 bits */
#define IV_SIZE 12 /* 96 bits as recommended by AES GCM spec */

int encrypt(char* plain, int plen, char* cipher, unsigned char* key, 
	    unsigned char* iv);
int decrypt(char* cipher, int clen, char* plain, unsigned char* key, 
	    unsigned char* iv);

#endif
