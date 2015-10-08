#include <stdio.h>
#include "shared/macros.h"
#include "atm/atm.h"
#include "bank/bank.h"

int main(int argc, char **argv)
{
#if defined BANK
    return bank_main(argc, argv);
#elif defined ATM
    return atm_main(argc, argv);
#else
    LOG("What binary am I even in?\n");
    return 1;
#endif
}
