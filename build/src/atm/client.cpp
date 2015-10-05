#include "client.h"

int atm_connect(char *host_name, int host_port)
{
    int hsock, *p_int, err;
    struct sockaddr_in my_addr;

    hsock = socket(AF_INET, SOCK_STREAM, 0);
    if (hsock == -1){
        ERR("Error initializing socket %d\n", errno);
        return 63;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if ( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        ERR("Error setting options %d\n", errno);
        free(p_int);
        return 63;
    }
    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr(host_name);

    if (connect(hsock, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        if ((err = errno) != EINPROGRESS) {
            ERR("Error connecting socket %d\n", errno);
            return 63;
        }
    }

    return hsock;
}

int atm_send(int hsock, char *buffer, unsigned buffer_len)
{
    int bytecount;

    if ((bytecount = send(hsock, buffer, strlen(buffer), 0)) == -1) {
        ERR("Error sending data %d\n", errno);
        return 63;
    }
    LOG("Sent bytes %d\n", bytecount);

    if ((bytecount = recv(hsock, buffer, buffer_len, 0)) == -1) {
        ERR("Error receiving data %d\n", errno);
        return 63;
    }
    LOG("Recieved bytes %d\n", bytecount);
    LOG("Received string \"%s\"\n", buffer);

    return 0;
}

void atm_close(int hsock)
{
    close(hsock);
}
