
#ifndef CLIENT_H
#define CLIENT_H

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
#include "shared/macros.h"
#include "shared/transfer.h"
#include "shared/config.h"

int atm_connect(char *host_name, int host_port);
int atm_send(int hsock, struct transfer *send_transfer, char *auth_file_contents);
void atm_close(int hsock);

#endif
