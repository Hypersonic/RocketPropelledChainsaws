#include "money.h"

void print_money(struct money *amt)
{
    printf("%u.%02u", amt->dollars, amt->cents);
}

int parse_money(struct money *amt, char *str)
{
    uint32_t dollars;
    uint8_t cents;
    unsigned parse_cents;
    char *decimal;

    if (strlen(str) < 4) {
        ERR("[-] Invalid money amount given: %s\n", str);
        return 0;
    }

    if (str[0] == '0' && str[1] != '.') {
        ERR("[-] Leading zero in given money: %s\n", str);
        return 0;
    }
    if ((decimal = strchr(str, '.')) == NULL) {
        ERR("[-] No decimal in: %s\n", str);
        return 0;
    }
    if (strlen(decimal) != 3) {
        ERR("[-] Invalid decimal given: %s\n", str);
        return 0;
    }
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
        if (UINT32_MAX - a->dollars < b.dollars) {
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
            if (UINT32_MAX - (a->dollars + 1) < b.dollars) {
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
            if ((a->dollars - 1) < b.dollars) {
				// Overflow when the carry would be applied
				return false;
			} else {
				a->dollars = (a->dollars - 1) - b.dollars;
				a->cents = (100 + a->cents) - b.cents;

				return true;
			}
		}
    }
    return true;
}

uint32_t compare_money(struct money *a, struct money *b) {
    uint32_t dollar_diff;

    if ((dollar_diff = a->dollars - b->dollars) == 0) {
        return (uint32_t) a->cents - b->cents;
    } else {
        return dollar_diff;
    }
}
