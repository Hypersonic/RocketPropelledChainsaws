#include <stdarg.h>
#include <stdio.h>
#include "money.h"

int add_money(money * a, money b) {
    uint32_t result;
    
    assert(a->cents < 100);
    assert(b.cents < 100);
    
    if ((a->cents + b.cents) < 100) {
        if (__builtin_uadd_overflow(a->dollars, b.dollars, &result)) {
            // Dollars overflow
            return 1;
        } else {
            a->dollars = result;
            a->cents = a->cents + b.cents;
            
            return 0;
        }
    } else {
        if (a->dollars == UINT32_MAX || b.dollars == UINT32_MAX) {
            // Overflow when 1 is added in carry
            return 1;
        } else {
            if (__builtin_uadd_overflow(a->dollars + 1, b.dollars, &result)) {
				// Overflow when the carry would be applied
				return 1;
			} else {
				a->dollars = result;
				a->cents = (a->cents + b.cents) % 100;
			
				return 0;
			}
		}
    }
}

// Subtracts b from a
int subtract_money(money * a, money b) {
    uint32_t result;
    
    assert(a->cents < 100);
    assert(b.cents < 100);
    
    if (b.cents <= a->cents) {
        if (__builtin_usub_overflow(a->dollars, b.dollars, &result)) {
            // Dollars overflow
            return 1;
        } else {
            a->dollars = result;
            a->cents = a->cents - b.cents;
            
            return 0;
        }
    } else {
        if (a->dollars == 0) {
            // Overflow from cents
            return 1;
        } else {
            if (__builtin_usub_overflow(a->dollars - 1, b.dollars, &result)) {
				// Overflow when the carry would be applied
				return 1;
			} else {
				a->dollars = result;
				a->cents = (100 + a->cents) - b.cents;
			
				return 0;
			}
		}
    }
}