#ifndef SERVE_H
#define SERVE_H
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
#include "shared/transfer.h"
#include "bank/db.h"

static db_t *db; /* global db */

void *bank_socket_handler(void *socket);

int bank_create_server(void *(*callback)(void *));

#endif
