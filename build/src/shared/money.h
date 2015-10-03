#ifndef MONEY_H
#define MONEY_H
#include <stdint.h>

struct money {
    uint32_t dollars;
    uint8_t  cents;
};

#endif
