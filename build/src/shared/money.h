#ifndef MONEY_H
#define MONEY_H
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <shared/macros.h>

struct money {
    uint32_t dollars;
    uint8_t  cents;
};

/* Adds money struct b to money pointer a
 * Returns whether the operation was a success
 */
bool add_money(struct money *a, struct money b);

/* Subtracts money struct b from money pointer a
 * Returns whether the operation was a success
 */
bool subtract_money(struct money *a, struct money b);

int parse_money(struct money *dst, char *str);

#endif
