#include <stdio.h>
#include "macros.h"

int main(UNUSED int argc, UNUSED char **argv)
{
    LOG("Hello, RocketPropelledChainsaws...\n");
    DEBUG("If you can see this, DEBUG_MODE is set properly! If not, export DEBUG in your environment and build again!\n");
    return 0;
}
