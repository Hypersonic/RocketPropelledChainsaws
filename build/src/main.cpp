#include <stdio.h>
#include "macros.h"

int main(UNUSED int argc, UNUSED char **argv)
{
    char a[128];
    LOG("Hello, RocketPropelledChainsaws...\n");
    putc(a[0], stdout);
    DEBUG("If you can see this, DEBUG_MODE is set properly! If not, export DEBUG in your environment and build again!\n");
    return 0;
}
