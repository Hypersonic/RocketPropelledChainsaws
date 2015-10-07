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

    my_addr.sin_family = AF_INET ;
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

int atm_send(int hsock, char *buffer, unsigned buffer_len)
{
    int bytecount;
    struct transfer *atm_transfer;
    char type;

    type = buffer[0];

    atm_transfer = (struct transfer *) malloc(sizeof(struct transfer));
    if (atm_transfer == NULL) {
        ERR("[-] Unable to allocate");
        atm_close(hsock);
        return 0;
    }

    if ((bytecount = send(hsock, buffer, buffer_len, 0)) == -1) {
        ERR("Error sending data %d\n", errno);
        atm_close(hsock);
        return 0;
    }
    LOG("Sent bytes %d\n", bytecount);

    if ((bytecount = recv(hsock, buffer, buffer_len, 0)) == -1) {
        ERR("Error receiving data %d\n", errno);
        atm_close(hsock);
        return 0;
    }
    LOG("Recieved bytes %d\n", bytecount);

    deserialize(atm_transfer, buffer);

    print_transfer(type, atm_transfer);

    free(atm_transfer);
    atm_close(hsock);
    return 1;
}

void atm_close(int hsock)
{
    close(hsock);
}
