#include "client.h"

int atm_connect(char *host_name, int host_port)
{
    int hsock, *p_int, err;
    struct sockaddr_in my_addr;

    if ((hsock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ERR("Error initializing socket %d\n", errno);
        return 0;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if ( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ) {
        ERR("Error setting options %d\n", errno);
        free(p_int);
        return 0;
    }
    free(p_int);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr(host_name);

    if (connect(hsock, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        if ((err = errno) != EINPROGRESS) {
            ERR("Error connecting socket %d\n", errno);
            return 0;
        }
    }

    return hsock;
}

int atm_send(int hsock, struct transfer *send_transfer, char *auth_file_contents)
{
    int bytecount;
    struct transfer *atm_transfer;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    char buffer[sizeof(struct transfer)];
    char tmp_nonce[NONCE_SIZE];
    int buffer_len = sizeof(struct transfer);

    unsigned char* iv;
    AES_RNG* rng_gen;
    char *c_txt;

    iv = (unsigned char*) malloc(NONCE_SIZE);


    if (setsockopt(hsock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof tv)) {
        ERR("[-] Unable to set timeout: %d\n", errno);
        goto FAIL;
    }
    if (setsockopt(hsock, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof tv)) {
        ERR("[-] Unable to set timeout: %d\n", errno);
        goto FAIL;
    }

    atm_transfer = (struct transfer *) malloc(sizeof(struct transfer));
    if (atm_transfer == NULL) {
        ERR("[-] Unable to allocate\n");
        goto FAIL;
    }

    if ((bytecount = recv(hsock, tmp_nonce, NONCE_SIZE, 0)) == -1) {
        ERR("[-] Error receiving data %d\n", errno);
        goto FAIL;
    }
    LOG("[+] Recieved bytes %d\n", bytecount);

    memcpy(&(send_transfer->nonce), tmp_nonce, NONCE_SIZE);

    /* initialize the rng */
    rng_gen = init_iv_gen((unsigned char*)tmp_nonce);
    get_next_iv(rng_gen,(char*)iv);

    serialize(buffer, send_transfer);

    if ((c_txt = (char *) malloc(sizeof(struct transfer))) == NULL) {
        ERR("[-] Error allocating\n");
        goto FAIL;
    }

    LOG("[+] Buffer Length: %d\n",buffer_len);

    if ((bytecount = secure_send(hsock, buffer, buffer_len, (unsigned char *) auth_file_contents, iv)) == -1) {
        ERR("[-] Error sending data %d\n", errno);
        goto FAIL;
    }
    LOG("[+] Sent bytes %d\n", bytecount);

    memset(iv, 0, NONCE_SIZE);
    get_next_iv(rng_gen, (char *) iv);

    if ((bytecount = secure_transfer_recv(hsock, buffer, buffer_len, (unsigned char *) auth_file_contents, iv)) == -1) {
        ERR("[-] Error decrypting data\n");
        goto FAIL;
    }

    LOG("[+] Recieved bytes %d\n", bytecount);

    if (send_transfer->type == 'g') {
        if (buffer[0] == '\xff') {
            goto SER_FAIL;
        }

        *(strchr(buffer, '.') + 3) = '\0';

        fputs("{\"account\":\"", stdout);
        print_escaped_string(send_transfer->name);
        fputs("\",\"balance\":", stdout);
        fputs(buffer, stdout);
        fputs("}\n", stdout);
        fflush(stdout);
    } else {

        deserialize(atm_transfer, buffer);
        LOG("Transfer type recieved: %d\n", atm_transfer->type);

        if (atm_transfer->type == 255) {
            goto SER_FAIL;
        }

        print_transfer(send_transfer->type, atm_transfer);
    }

    free(atm_transfer);
    atm_close(hsock);
    return 1;
SER_FAIL:
    free(atm_transfer);
    atm_close(hsock);
    return 255;
FAIL:
    atm_close(hsock);
    return 63;
}

void atm_close(int hsock)
{
    close(hsock);
}
