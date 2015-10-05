#include "atm.h"

int atm_main(UNUSED int argc, UNUSED char **argv)
{
    LOG("Hello, ATM!\n");

    client_connect();
    return 0;
}
