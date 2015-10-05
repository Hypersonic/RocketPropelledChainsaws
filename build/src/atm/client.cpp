#include "client.h"

int client_connect()
{
    int bytecount, buffer_len, hsock, *p_int, err, host_port;
    char buffer[1024];
    const char *host_name;
    struct sockaddr_in my_addr;

    host_port = 1337;
    buffer_len = 0;
    host_name = "127.0.0.1";

    hsock = socket(AF_INET, SOCK_STREAM, 0);
    if (hsock == -1){
        ERR("Error initializing socket %d\n", errno);
        return 255;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if ( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        ERR("Error setting options %d\n", errno);
        free(p_int);
        return 255;
    }
    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr(host_name);

    if (connect(hsock, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        if ((err = errno) != EINPROGRESS) {
            ERR("Error connecting socket %d\n", errno);
            return 255;
        }
    }

    buffer_len = 1024;

    memset(buffer, '\0', buffer_len);

    LOG("Enter some text to send to the server (press enter)\n");
    fgets(buffer, 1024, stdin);
    buffer[strlen(buffer)-1]='\0';

    if ((bytecount = send(hsock, buffer, strlen(buffer), 0)) == -1) {
        ERR("Error sending data %d\n", errno);
        return 255;
    }
    LOG("Sent bytes %d\n", bytecount);

    if ((bytecount = recv(hsock, buffer, buffer_len, 0)) == -1) {
        ERR("Error receiving data %d\n", errno);
        return 255;
    }
    LOG("Recieved bytes %d\n", bytecount);
    LOG("Received string \"%s\"\n", buffer);

    close(hsock);
    return 255;
}
