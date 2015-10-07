#include "server.h"

int bank_create_server()
{
    struct sockaddr_in my_addr;
    int hsock, *p_int, *csock;
    sockaddr_in sadr;

    socklen_t addr_size = 0;
    pthread_t thread_id = 0;

    if ((hsock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        ERR("[-] Error initializing socket %d\n", errno);
        return 255;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        ERR("[-] Error setting options %d\n", errno);
        free(p_int);
        return 255;
    }

    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(BANK_PORT);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

    if (bind(hsock, (sockaddr *) &my_addr, sizeof(my_addr)) == -1){
        ERR("[-] Error binding to socket, make sure nothing else is listening on \
             this port: %d, %d\n", BANK_PORT, errno);
        return 255;
    }
    if (listen(hsock, 10) == -1){
        ERR("[-] Error listening %d\n", errno);
        return 255;
    }

    addr_size = sizeof(sockaddr_in);

    while (true) {
        DEBUG("[*] Waiting for a connection\n");
        csock = (int *) malloc(sizeof(int));
        if ((*csock = accept(hsock, (sockaddr *)&sadr, &addr_size)) != -1) {
            DEBUG("---------------------\nReceived connection from %s\n", inet_ntoa(sadr.sin_addr));
            pthread_create(&thread_id, 0, &bank_socket_handler, (void *)csock);
            pthread_detach(thread_id);
        } else {
            ERR("[-] Error accepting %d\n", errno);
            puts("protocol_error");
            fflush(stdout);
            return 255;
        }
    }
}

void *bank_socket_handler(void *lp)
{
    int *csock = (int *) lp;
    struct transfer *trans;

    char buffer[1024];
    int buffer_len = 1024;
    int bytecount;

    memset(buffer, 0, buffer_len);
    if((bytecount = recv(*csock, buffer, buffer_len, 0)) == -1){
        ERR("Error receiving data %d\n", errno);
        puts("protocol_error");
        fflush(stdout);
        server_close(csock);
        return NULL;
    }

    if (NULL == (trans = (struct transfer *) malloc(sizeof(struct transfer)))) {
        ERR("Error allocating transfer struct\n");
        puts("protocol_error");
        fflush(stdout);
        /* TODO: send a response back to the client */
        server_close(csock);
        return NULL;
    }

    if (!deserialize(trans, buffer)) {
        ERR("Error deserializing transfer struct\n");
        puts("protocol_error");
        fflush(stdout);
        /* TODO: send a response back to the client */
        server_close(csock);
        return NULL;
    }

    switch (trans->type) {
    case 'n': /* new account */
        if (!db_contains(db, trans->name)) {
            ERR("User already exists: \"%s\"\n", trans->name);
            /* TODO: send a response back to the client */
            server_close(csock);
            return NULL;
        }
        db_insert(db, trans->name, trans->amt);
        /* TODO: print transfer */
        trans->type = 0; /* return code 0 */
        serialize(buffer, trans);
        break;
    case 'd': /* deposit */
        break;
    case 'w': /* withdraw */
        break;
    case 'g': /* get balance */
        break;
    default:  /* Error */
        ERR("Error deserializing transfer struct\n");
        puts("protocol_error");
        fflush(stdout);
        /* TODO: send a response back to the client */
        server_close(csock);
        return NULL;
    }

    if((bytecount = send(*csock, buffer, strlen(buffer), 0)) == -1){
        ERR("Error sending data %d\n", errno);
        puts("protocol_error");
        fflush(stdout);
        server_close(csock);
        return NULL;
    }

    DEBUG("Sent bytes %d\n", bytecount);

    server_close(csock);
    return 0;
}

void server_close(int *csock)
{
    close(*csock);
    free(csock);
}
