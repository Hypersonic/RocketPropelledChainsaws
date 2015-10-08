#include "server.h"

int bank_create_server(int host_port)
{
    struct sockaddr_in my_addr;
    int hsock, *p_int, *csock;
    sockaddr_in sadr;

    db = db_create(); /* initialize the global db */

    socklen_t addr_size = 0;
    pthread_t thread_id = 0;

    if ((hsock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        ERR("[-] Error initializing socket %d\n", errno);
        return 0;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        ERR("[-] Error setting options %d\n", errno);
        free(p_int);
        return 0;
    }

    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

    if (bind(hsock, (sockaddr *) &my_addr, sizeof(my_addr)) == -1){
        ERR("[-] Error binding to socket, make sure nothing else is listening on \
             this port: %d, %d\n", host_port, errno);
        return 0;
    }
    if (listen(hsock, 10) == -1){
        ERR("[-] Error listening %d\n", errno);
        return 0;
    }

    addr_size = sizeof(sockaddr_in);

    while (true) {
        DEBUG("[*] Waiting for a connection\n");

        csock = (int *) malloc(sizeof(int));
        if ((*csock = accept(hsock, (sockaddr *)&sadr, &addr_size)) != -1) {
            DEBUG("[+] Received connection from %s\n", inet_ntoa(sadr.sin_addr));
            pthread_create(&thread_id, 0, &bank_socket_handler, (void *)csock);
            pthread_detach(thread_id);
        } else {
            ERR("[-] Error accepting %d\n", errno);
            puts("protocol_error");
            fflush(stdout);
            return 0;
        }
    }
}

void *bank_socket_handler(void *lp)
{
    uint32_t nonce;
    int bytecount, buffer_len, *csock;
    struct transfer *trans;
    struct money curr_balance;
    struct timeval tv;

    char buffer[sizeof(struct transfer)];
    char tmp_nonce[NONCE_SIZE];

    tv.tv_sec = 10;
    tv.tv_usec = 0;
    csock = (int *) lp;
    buffer_len = sizeof(struct transfer);

    if (setsockopt(*csock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof tv)) {
        ERR("[-] Unable to set timeout: %d\n", errno);
        return NULL;
    }

    random_bytes(tmp_nonce, NONCE_SIZE);
    memcpy(&nonce, tmp_nonce, NONCE_SIZE);
    if((bytecount = send(*csock, tmp_nonce, NONCE_SIZE, 0)) == -1){
        ERR("Error sending data %d\n", errno);
        goto NET_FAIL;
    }

    /* TODO: add nonce to database */

    memset(buffer, 0, buffer_len);
    if((bytecount = recv(*csock, buffer, buffer_len, 0)) == -1){
        ERR("Error receiving data %d\n", errno);
        goto NET_FAIL;
    }

    if (NULL == (trans = (struct transfer *) malloc(sizeof(struct transfer)))) {
        ERR("Error allocating transfer struct\n");
        goto SER_FAIL;
    }

    if (bytecount != sizeof(struct transfer)
        || !deserialize(trans, buffer)) {
        ERR("Error deserializing transfer struct\n");
        goto SER_FAIL;
    }

    /* TODO: check nonce */

    switch (trans->type) {
    case 'n': /* new account */
        if (db_contains(db, trans->name)) {
            ERR("User already exists: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        db_insert(db, trans->name, trans->amt);
        print_transfer(trans->type, trans);
        trans->type = 0; /* return code 0 */
        break;
    case 'd': /* deposit */
        if (!db_contains(db, trans->name)) {
            ERR("No such user: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        curr_balance = db_get(db, trans->name);
        if (!add_money(&curr_balance, trans->amt)) {
            ERR("Balance would overflow, not adding\n");
            goto SER_FAIL;
        }
        if (!db_update(db, trans->name, curr_balance)) {
            ERR("Could not update DB with new balance, aborting!\n");
            goto SER_FAIL;
        }
        print_transfer(trans->type, trans);
        trans->type = 0; /* return code 0 */
        break;
    case 'w': /* withdraw */
        if (!db_contains(db, trans->name)) {
            ERR("No such user: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        curr_balance = db_get(db, trans->name);
        if (!subtract_money(&curr_balance, trans->amt)) {
            ERR("Balance would overflow, not subtracting\n");
            goto SER_FAIL;
        }
        if (!db_update(db, trans->name, curr_balance)) {
            ERR("Could not update DB with new balance, aborting!\n");
            goto SER_FAIL;
        }
        print_transfer(trans->type, trans);
        trans->type = 0; /* return code 0 */
        break;
    case 'g': /* get balance */
        if (!db_contains(db, trans->name)) {
            ERR("No such user: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        curr_balance = db_get(db, trans->name);
        trans->amt = curr_balance;
        print_transfer(trans->type, trans);
        trans->type = 0; /* return code 0 */
        break;
    default:  /* Error */
        ERR("Error deserializing transfer struct, unknown option %c\n", trans->type);
        goto SER_FAIL;
    }
    serialize(buffer, trans);

    if((bytecount = send(*csock, buffer, buffer_len, 0)) == -1){
        ERR("Error sending data %d\n", errno);
        goto NET_FAIL;
    }

    DEBUG("Sent bytes %d\n", bytecount);

    server_close(csock);
    return NULL;

SER_FAIL:
    puts("protocol_error");
    fflush(stdout);

    trans->type = 255;
    serialize(buffer, trans);
    if((bytecount = send(*csock, buffer, buffer_len, 0)) == -1){
        ERR("Error sending data %d\n", errno);
    }
NET_FAIL:
    server_close(csock);
    return NULL;
}

void server_close(int *csock)
{
    close(*csock);
    free(csock);
}
