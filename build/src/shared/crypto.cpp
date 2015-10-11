#include "crypto.h"
using CryptoPP::AES;
using CryptoPP::GCM;

/* encrypts and authenticates plain using key and iv, stores result in cipher */
/* returns 1 on success, 0 on failure */
/* ciphertext length = plen + TAG_SIZE */
int encrypt(char* plain, int plen, char* cipher, unsigned char* key, 
	    unsigned char* iv) {
    try {
	GCM<AES>::Encryption e;
	e.SetKeyWithIV(key, KEY_SIZE, iv, IV_SIZE);
	e.EncryptAndAuthenticate((unsigned char*)cipher,
				 (unsigned char*)cipher + plen,
				 TAG_SIZE,
				 iv,
				 IV_SIZE,
				 NULL,
				 0,
				 (unsigned char*)plain,
				 plen);
	return 1;
    }
    catch(CryptoPP::Exception& e) {
	ERR("[-] Caught CryptoPP::Exception: %s\n", e.what());
	//    exit(1);
	return 0;
    }
}

/* decrypts and verifies cipher using key and iv, stores result in plain */
/* returns 1 on success, 0 on failure */
int decrypt(char* cipher, int clen, char* plain, unsigned char* key, 
	    unsigned char* iv) {
    try {
	GCM<AES>::Decryption d;
	d.SetKeyWithIV(key, KEY_SIZE, iv, IV_SIZE);
	int status = d.DecryptAndVerify((unsigned char*)plain,
					(unsigned char*)cipher + (clen - TAG_SIZE),
					TAG_SIZE,
					iv,
					IV_SIZE,
					NULL,
					0,
					(unsigned char*)cipher,
					clen - TAG_SIZE);
	return status;
    }
    catch(CryptoPP::Exception& e) {
	ERR("[-] Caught CryptoPP::Exception: %s\n", e.what());
	// exit(2);
	return 0;
    }
}
