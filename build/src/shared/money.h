#ifndef MONEY_H
#define MONEY_H
#include <assert.h>
#include <stdint.h>

struct money {
    uint32_t dollars;
    uint8_t  cents;
};

int add_money(money * a, money b);
int subtract_money(money * a, money b);

#endif
