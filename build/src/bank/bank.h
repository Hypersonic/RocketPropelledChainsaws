#ifndef BANK_H
#define BANK_H
#include "shared/macros.h"
#include "shared/util.h"
#include "db.h"
#include "server.h"

struct bank_client {
    int *socket;
};

int bank_main(int argc, char **argv);
int bank_create_server();
void *bank_socket_handler(void *socket);

#endif
