#include "server.h"

static unsigned char *key;
db_t* db;

int bank_create_server(int host_port, unsigned char *auth_file_contents)
{
    struct sockaddr_in my_addr;
    int hsock, *p_int, *csock;
    sockaddr_in sadr;

    db = db_create(); /* initialize the global db */

    key = auth_file_contents;

    socklen_t addr_size = 0;
    pthread_t thread_id = 0;

    if ((hsock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        ERR("[-] Error initializing socket %d\n", errno);
        return 0;
    }

    p_int = (int *) malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char *)p_int, sizeof(int)) == -1 ) ||
        (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char *)p_int, sizeof(int)) == -1 ) ){
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
    char nonce[NONCE_SIZE];
    int bytecount, buffer_len, *csock, i;
    struct transfer *trans;
    struct timeval tv;
    struct db_money curr_balance;
    struct db_money tmp_money;
    std::string tmp_cents;
    BigUnsigned tmp_big;
    AES_RNG* rng_gen;

    const char *db_card, *trans_card;

    char buffer[sizeof(struct transfer)];
    std::string big_int;

    char *c_txt;
    unsigned char *iv;

    /* iv will hold new ivs as the come */
    iv = (unsigned char*) malloc(NONCE_SIZE);
    c_txt = (char*) malloc(sizeof(struct transfer));

    tv.tv_sec = 10;
    tv.tv_usec = 0;
    csock = (int *) lp;
    buffer_len = sizeof(struct transfer);

    if (setsockopt(*csock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof tv)) {
        ERR("[-] Unable to set timeout: %d\n", errno);
        return NULL;
    }
    if (setsockopt(*csock, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof tv)) {
        ERR("[-] Unable to set timeout: %d\n", errno);
        return NULL;
    }

    /* Generate a unique nonce */
    do {
        random_bytes(nonce, NONCE_SIZE);
    } while (db_nonce_contains(db, nonce));
    
    /* seed our iv generator */
    rng_gen = init_iv_gen((unsigned char*) nonce);
    
    if(!get_next_iv(rng_gen, (char*)iv)){
        ERR("Failed to get next iv\n");
	goto NET_FAIL;
    }
    
    /* insert into DB */
    db_nonce_insert(db, nonce, true);

    if ((bytecount = send(*csock, nonce, NONCE_SIZE, 0)) == -1){
        ERR("Error sending data %d\n", errno);
        goto NET_FAIL;
    }
    
    memset(buffer, 0, buffer_len);
    if ((bytecount = secure_transfer_recv(*csock, buffer, buffer_len, key, iv)) == -1){
        ERR("Error receiving data %d\n", errno);
        goto NET_FAIL;
    }
    
    DEBUG("Recieved encrypted data; Length %d\n",bytecount);
    DEBUG("Buffer Length %d\n",buffer_len);
    
    if (NULL == (trans = (struct transfer *) malloc(sizeof(struct transfer)))) {
        ERR("Error allocating transfer struct\n");
        goto SER_FAIL;
    }

    if (bytecount != sizeof(struct transfer)
        || !deserialize(trans, buffer)) {
        ERR("Error deserializing transfer struct\n");
        puts("protocol_error");
        fflush(stdout);
        goto NET_FAIL;
    }

    if (!db_nonce_contains(db, trans->nonce)) {
        ERR("Nonce mismatch, aborting!\n");
        goto SER_FAIL;
    } else {
        db_nonce_remove(db, trans->nonce);
    }

    if (db_card_contains(db, trans->name)) {
        db_card = db_card_get(db, trans->name).c_str();
        trans_card = trans->card;

        i = 0;
        while (i < CARD_SIZE) {
            if (db_card[i] != trans_card[i]) break;
            i++;
        }
        if (i != CARD_SIZE) {
            ERR("Card mismatch, aborting!\n");
            goto SER_FAIL;
        }
    } else {
        if (!db_card_insert(db, trans->name, trans->card)) {
            goto SER_FAIL;
        }
    }

    switch (trans->type) {
    case 'n': /* new account */
        if (db_contains(db, trans->name)) {
            ERR("User already exists: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        tmp_money.dollars = trans->amt.dollars;
        tmp_money.cents = trans->amt.cents;
        db_insert(db, trans->name, tmp_money);
        print_transfer(trans->type, trans);
        trans->type = 0; /* return code 0 */
        break;
    case 'd': /* deposit */
        if (!db_contains(db, trans->name)) {
            ERR("No such user: \"%s\"\n", trans->name);
            goto SER_FAIL;
        }
        curr_balance = db_get(db, trans->name);
        add_money(&curr_balance, trans->amt);
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
        tmp_cents = std::to_string(curr_balance.cents);
        tmp_big = curr_balance.dollars;
        big_int = bigUnsignedToString(tmp_big);
        big_int += "." + (tmp_cents.length() == 1 ? std::string("0") : std::string("")) + tmp_cents;
        break;
    default:  /* Error */
        ERR("Error deserializing transfer struct, unknown option %c\n", trans->type);
        goto SER_FAIL;
    }

    if (trans->type == 'g') {
        const char *tmp = big_int.c_str();
	memset(iv,0,NONCE_SIZE);
	
	if(!get_next_iv(rng_gen, (char*) iv)){
	    ERR("Failed to generate a new iv\n");
	    goto NET_FAIL;
	}

        if((bytecount = secure_send(*csock, tmp, big_int.length(), key, iv)) == -1){
            ERR("Error sending data %d\n", errno);
            goto NET_FAIL;
        }

        printf("{\"account\":\"");
        print_escaped_string(trans->name);
        printf("\",\"balance\":%s}\n", tmp);
        fflush(stdout);
    } else {
        serialize(buffer, trans);
	
	memset(iv,0,NONCE_SIZE);
	
	if(!get_next_iv(rng_gen, (char*) iv)){
	    ERR("Failed to generate a new iv\n");
	    goto NET_FAIL;
	}
	
        if((bytecount = secure_send(*csock, buffer, buffer_len, key, (unsigned char*)iv)) == -1){
            ERR("Error sending data %d\n", errno);
	    goto NET_FAIL;
        }
    }

    DEBUG("Sent bytes %d\n", bytecount);

    server_close(csock);
    return NULL;

SER_FAIL:
    trans->type = 255;
    serialize(buffer, trans);
    
    memset(iv,0,NONCE_SIZE);
	
    if(!get_next_iv(rng_gen, (char*) iv)){
	ERR("Failed to generate a new iv\n");
	goto NET_FAIL;
    }

    if((bytecount = secure_send(*csock, c_txt, buffer_len, key, iv)) == -1){
        ERR("Error sending data %d\n", errno);
    }

    free(trans);
    server_close(csock);
    return NULL;
NET_FAIL:
    db_nonce_remove(db, nonce);
    puts("protocol_error");
    fflush(stdout);

    server_close(csock);
    return NULL;
}

void server_close(int *csock)
{
    close(*csock);
    free(csock);
}
