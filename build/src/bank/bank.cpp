#include "bank.h"

/* Test duplicate inserts */
UNUSED int test_db() {
    db_t *db = db_create();
    if (!db_insert(db, "asdf", {100, 1})) {
        return 255;
    }
    if (!db_update(db, "asdf", {100, 2})) {
        return 255;
    }
    if (!db_insert(db, "asdf", {100, 3})) {
        return 255;
    }
    db_destroy(db);
    return 0;
}

int bank_main(UNUSED int argc, UNUSED char **argv)
{

    LOG("Hello, Bank!\n");

    bank_create_server();
    return 0;
}
