#include <stdio.h>
#include "shared/macros.h"
#include "atm/atm.h"
#include "bank/bank.h"

int main(int argc, char **argv)
{
#if defined BANK
    bank_main(argc, argv);
#elif defined ATM
    atm_main(argc, argv);
#else
    LOG("What binary am I even in?\n");
#endif
    DEBUG("If you can see this, DEBUG_MODE is set properly! If not, export DEBUG in your environment and build again!\n");
    return 0;
}
