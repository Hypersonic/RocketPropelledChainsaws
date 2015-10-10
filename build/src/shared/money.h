#ifndef MONEY_H
#define MONEY_H
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <shared/macros.h>
#include <shared/lib/BigIntegerLibrary.h>

struct money {
    uint32_t dollars;
    uint8_t  cents;
};

struct db_money {
    BigUnsigned dollars;
    uint8_t  cents;
};

/* Adds money struct b to money pointer a
 * Returns whether the operation was a success
 */
void add_money(struct db_money *a, struct money b);

/* Subtracts money struct b from money pointer a
 * Returns whether the operation was a success
 */
bool subtract_money(struct db_money *a, struct money b);

int parse_money(struct money *dst, char *str);
void print_money(struct money *amt);

#endif
