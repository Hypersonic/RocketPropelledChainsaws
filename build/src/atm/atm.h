#ifndef ATM_H
#define ATM_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared/macros.h"
#include "shared/util.h"
#include "shared/money.h"
#include "client.h"

int atm_main(int argc, char **argv);

#endif
