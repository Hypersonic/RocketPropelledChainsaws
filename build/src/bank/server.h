#ifndef SERVE_H
#define SERVE_H
#include <string>

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "shared/macros.h"
#include "shared/config.h"
#include "shared/crypto.h"
#include "shared/transfer.h"
#include "shared/lib/BigIntegerUtils.h"
#include "bank/db.h"

/* not actually unused but compiler on linux complains */
UNUSED static db_t *db; /* global db */

void *bank_socket_handler(void *socket);

int bank_create_server(int host_port, char* auth_file);

void server_close(int *csock);

#endif
