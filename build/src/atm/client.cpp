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
    char *big_int = NULL;
    int buffer_len = sizeof(struct transfer);

    char *c_txt;

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

    serialize(buffer, send_transfer);

    if ((c_txt = (char *) malloc(sizeof(struct transfer))) == NULL) {
        ERR("[-] Error allocating\n");
        goto FAIL;
    }

    if (!encrypt(buffer, buffer_len, c_txt, (unsigned char *) auth_file_contents,
                (unsigned char*) tmp_nonce)) {
        ERR("Failed to encrypt serialized data\n");
    	goto FAIL;
    }

    LOG("[+] Serialized data encrypted\n");
    LOG("[+] Buffer Length: %d\n",buffer_len);

    if ((bytecount = send(hsock, c_txt, buffer_len, 0)) == -1) {
        ERR("[-] Error sending data %d\n", errno);
        goto FAIL;
    }
    LOG("[+] Sent bytes %d\n", bytecount);

    if (send_transfer->type == 'g') {
        if ((big_int = recv_var_bytes(hsock, &bytecount)) == NULL) {
            ERR("[-] Error receiving data %d\n", errno);
            goto FAIL;
        }
    } else {
        if ((bytecount = recv(hsock, buffer, buffer_len, 0)) == -1) {
            ERR("[-] Error receiving data %d\n", errno);
            goto FAIL;
        }
    }
    LOG("[+] Recieved bytes %d\n", bytecount);

    if (send_transfer->type == 'g') {
        if (big_int[0] == '\xff') {
            goto SER_FAIL;
        }

        printf("{\"account\":\"");
        print_escaped_string(send_transfer->name);
        printf("\",\"balance\":%s}\n", big_int);
        fflush(stdout);

        free(big_int);
    } else {

        deserialize(atm_transfer, buffer);
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
