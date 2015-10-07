#include "money.h"

void print_money(struct money *amt)
{
    printf("%u.%u", amt->dollars, amt->cents);
}

int parse_money(struct money *amt, char *str)
{
    uint32_t dollars;
    uint8_t cents;
    unsigned parse_cents;

    sscanf(str, "%u.%u", &dollars, &parse_cents);
    if (parse_cents >= 100) {
        ERR("[-] Invalid cents amount entered: %u\n", parse_cents);
        return 0;
    }
    cents = (uint8_t) parse_cents;

    amt->dollars = dollars;
    amt->cents = cents;

    LOG("[+] Parsed money: %u.%u\n", dollars, cents);

    return 1;
}

bool add_money(struct money *a, struct money b)
{
    uint32_t result;

    assert(a->cents < 100);
    assert(b.cents < 100);

    if ((a->cents + b.cents) < 100) {
        if (__builtin_uadd_overflow(a->dollars, b.dollars, &result)) {
            // Dollars overflow
            return false;
        } else {
            a->dollars = result;
            a->cents = a->cents + b.cents;

            return true;
        }
    } else {
        if (a->dollars == UINT32_MAX || b.dollars == UINT32_MAX) {
            // Overflow when 1 is added in carry
            return false;
        } else {
            if (__builtin_uadd_overflow(a->dollars + 1, b.dollars, &result)) {
				// Overflow when the carry would be applied
				return false;
			} else {
				a->dollars = result;
				a->cents = (a->cents + b.cents) % 100;

				return true;
			}
		}
    }
}

bool subtract_money(struct money *a, struct money b)
{
    uint32_t result;

    assert(a->cents < 100);
    assert(b.cents < 100);

    if (b.cents <= a->cents) {
        if (__builtin_usub_overflow(a->dollars, b.dollars, &result)) {
            // Dollars overflow
            return false;
        } else {
            a->dollars = result;
            a->cents = a->cents - b.cents;

            return true;
        }
    } else {
        if (a->dollars == 0) {
            // Overflow from cents
            return false;
        } else {
            if (__builtin_usub_overflow(a->dollars - 1, b.dollars, &result)) {
				// Overflow when the carry would be applied
				return false;
			} else {
				a->dollars = result;
				a->cents = (100 + a->cents) - b.cents;

				return true;
			}
		}
    }
}
