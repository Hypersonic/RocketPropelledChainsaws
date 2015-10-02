#include <stdio.h>
#include "shared/macros.h"

int main(UNUSED int argc, UNUSED char **argv)
{
#if defined BANK
    LOG("Hello, Bank!\n");
#elif defined ATM
    LOG("Hello, ATM!\n");
#else
    LOG("What binary am I even in?\n");
#endif
    DEBUG("If you can see this, DEBUG_MODE is set properly! If not, export DEBUG in your environment and build again!\n");
    return 0;
}
