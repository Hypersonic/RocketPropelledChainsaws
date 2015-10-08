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
    assert(a->cents < 100);
    assert(b.cents < 100);

    if ((a->cents + b.cents) < 100) {
        if (UINT32_MAX - a->dollars > b.dollars) {
            // Dollars overflow
            return false;
        } else {
            a->dollars = a->dollars + b.dollars;
            a->cents = a->cents + b.cents;

            return true;
        }
    } else {
        if (a->dollars == UINT32_MAX || b.dollars == UINT32_MAX) {
            // Overflow when 1 is added in carry
            return false;
        } else {
            if (UINT32_MAX - (a->dollars + 1) > b.dollars) {
				// Overflow when the carry would be applied
				return false;
			} else {
				a->dollars = (a->dollars + 1) + b.dollars;
				a->cents = (a->cents + b.cents) % 100;

				return true;
			}
		}
    }
    return true;
}

bool subtract_money(struct money *a, struct money b)
{
    assert(a->cents < 100);
    assert(b.cents < 100);

    if (b.cents <= a->cents) {
        if (a->dollars < b.dollars) {
            // Dollars overflow
            return false;
        } else {
            a->dollars = a->dollars - b.dollars;
            a->cents = a->cents - b.cents;

            return true;
        }
    } else {
        if (a->dollars == 0) {
            // Overflow from cents
            return false;
        } else {
            if ((a->dollars + 1) < b.dollars) {
				// Overflow when the carry would be applied
				return false;
			} else {
				a->dollars = (a->dollars + 1) - b.dollars;
				a->cents = (100 + a->cents) - b.cents;

				return true;
			}
		}
    }
    return true;
}
