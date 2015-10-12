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
        return 0;
    }
}


ssize_t secure_send(int sockfd, const void* buf, size_t len,
    unsigned char* key, unsigned char* iv) {
    int enc_len = len + TAG_SIZE;
    char enc[enc_len];
    encrypt((char *)buf, len, enc, key, iv);
    return send(sockfd, enc, enc_len, 0);
}


ssize_t secure_recv(int sockfd, void* buf, unsigned char* key,
    unsigned char* iv) {
    int len, enc_len, status;
    char *enc = (char *)recv_var_bytes(sockfd, &enc_len);
    if(enc == NULL) {
    /* if there is an error recv_var_bytes will report it for us */
        return -1;
    }
    len = enc_len - TAG_SIZE;
    char *rec = (char *)malloc(len);
    if(rec == NULL) {
        ERR("[-] Unable to allocate\n");
        return -1;
    }
    status = decrypt((char *)buf, enc_len, rec, key, iv);
    return status ? len : -1;
}

AES_RNG* init_iv_gen(unsigned char* iv) {
    return new AES_RNG(iv, IV_SIZE);
}

int get_next_iv(AES_RNG* prng, unsigned char* iv) {
    prng->GenerateBlock(iv, IV_SIZE);
    #if defined BANK
    //    if(db_nonce_contains(db, 

    #endif
    return 0;
}

void free_iv_gen(AES_RNG* prng) {
    free(prng);
}
