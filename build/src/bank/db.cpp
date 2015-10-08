#include "db.h"

db_t *db_create()
{
    db_t *db;

    if (NULL == (db = (db_t *) malloc(sizeof(db_t)))) {
        ERR("Out of memory, can't allocate DB\n");
        return NULL;
    }

    if (0 != pthread_mutex_init(&db->balance_lock, NULL)) {
        ERR("Could not initialize mutex on DB\n");
        free(db);
        return NULL;
    }

    db->balances = new std::map<std::string, struct money>();

    return db;
}

bool db_destroy(db_t *db)
{
    if (0 != pthread_mutex_destroy(&db->balance_lock)) {
        ERR("Could not destroy DB mutex\n");
        return false;
    }

    delete db->balances;

    return true;
}

bool db_insert(db_t *db, std::string key, struct money value)
{
    assert(db != NULL);


    if (0 != pthread_mutex_lock(&db->balance_lock)) {
        LOG("Could not lock db\n");
        return false;
    }

    /* if the db contains the key, we return an error */
    if (db->balances->end() != db->balances->find(key)) {
        LOG("DB contained key \"%s\" already, cannot insert\n", key.c_str());
        return false;
    }

    DEBUG("Inserting into DB, key \"%s\": $%u.%d\n", key.c_str(), value.dollars, value.cents);
    (*db->balances)[key] = value; /* insert the element */

    if (0 != pthread_mutex_unlock(&db->balance_lock)) {
        LOG("Could not unlock db\n");
        return false;
    }

    return true;
}

bool db_update(db_t *db, std::string key, struct money value)
{
    assert(db != NULL);

    if (0 != pthread_mutex_lock(&db->balance_lock)) {
        LOG("Could not lock db\n");
        return false;
    }

    /* if the db does not contain the key, we return an error */
    if (db->balances->end() == db->balances->find(key)) {
        LOG("DB did not contain key \"%s\", cannot update\n", key.c_str());
        return false;
    }

    DEBUG("Updating DB, key \"%s\": $%u.%d -> $%u.%d\n", key.c_str(), (*db->balances)[key].dollars, (*db->balances)[key].cents, value.dollars, value.cents);
    (*db->balances)[key] = value; /* change the element */

    if (0 != pthread_mutex_unlock(&db->balance_lock)) {
        LOG("Could not unlock db\n");
        return false;
    }

    return true;
}

bool db_contains(db_t *db, std::string key)
{
    assert(db != NULL);

    return db->balances->end() != db->balances->find(key);
}

struct money db_get(db_t *db, std::string key)
{
    assert(db != NULL);

    return (*db->balances)[key];
}
