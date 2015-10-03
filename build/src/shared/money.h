#ifndef MONEY_H
#define MONEY_H
#include <assert.h>
#include <stdint.h>

struct money {
    uint32_t dollars;
    uint8_t  cents;
};

bool add_money(money * a, money b);
bool subtract_money(money * a, money b);

#endif
