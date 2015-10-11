#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include "shared/macros.h"
#include "shared/config.h"

int is_valid_name(char *name);
int is_valid_filename(char *filename);

void print_escaped_string(char *str);
int random_bytes(char* buf, size_t len);

unsigned get_file_size(const char * file_name);
int read_from_file(char *dst, unsigned read_size, char *file_name);
int write_to_file(char *src, unsigned write_size, char *file_name);

void* recv_var_bytes(int sock);

#endif
