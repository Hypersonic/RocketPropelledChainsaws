#include "bank.h"

int bank_main(UNUSED int argc, UNUSED char **argv)
{
    LOG("Hello, Bank!\n");
    db_t *db = db_create();
    /* Test duplicate inserts */
    if (!db_insert(db, "asdf", {100, 1})) {
        return 255;
    }
    if (!db_insert(db, "asdf", {100, 1})) {
        return 255;
    }
    return 0;
}
