#include "db.h"

db_t *db_create()
{
    db_t *db;

    if (NULL == (db = (db_t *) malloc(sizeof(db_t)))) {
        ERR("Out of memory, can't allocate DB\n");
        return NULL;
    }

    if (0 == pthread_mutex_init(&db->lock, NULL)) {
        ERR("Could not initialize mutex on DB\n");
        free(db);
        return NULL;
    }

    db->balances = std::map<std::string, struct money>();

    return db;
}
