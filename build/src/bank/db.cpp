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

    if (0 != pthread_mutex_init(&db->nonce_lock, NULL)) {
        ERR("Could not initialize mutex on DB\n");
        free(db);
        return NULL;
    }

    db->balances = new std::map<std::string, struct db_money>();
    db->nonces = new std::map<std::string, bool>();
    db->cards = new std::map<std::string, std::string>();

    return db;
}

bool db_destroy(db_t *db)
{
    if (0 != pthread_mutex_destroy(&db->balance_lock)) {
        ERR("Could not destroy DB mutex\n");
        return false;
    }

    if (0 != pthread_mutex_destroy(&db->nonce_lock)) {
        ERR("Could not destroy DB mutex\n");
        return false;
    }

    delete db->balances;
    delete db->nonces;
    delete db->cards;

    free(db);

    return true;
}

bool db_insert(db_t *db, std::string key, struct db_money &value)
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

    /*
    UNUSED const char *v1 = bigUnsignedToString(value.dollars).c_str();
    DEBUG("Inserting into DB, key \"%s\": $%s.%d\n", key.c_str(), v1, value.cents);
    */
    (*db->balances)[key] = value; /* insert the element */

    if (0 != pthread_mutex_unlock(&db->balance_lock)) {
        LOG("Could not unlock db\n");
        return false;
    }

    return true;
}

bool db_update(db_t *db, std::string key, struct db_money &value)
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
    /*
    UNUSED const char *v1 = bigUnsignedToString((*db->balances)[key].dollars).c_str();
    UNUSED const char *v2 = bigUnsignedToString(value.dollars).c_str();
    DEBUG("Updating DB, key \"%s\": $%s.%d -> $%s.%d\n", key.c_str(),
        v1, (*db->balances)[key].cents, v2, value.cents);
    */
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

struct db_money db_get(db_t *db, std::string key)
{
    assert(db != NULL);

    return (*db->balances)[key];
}

bool db_nonce_insert(db_t *db, char *keyp, bool value)
{
    assert(db != NULL);

    std::string key(keyp, NONCE_SIZE);

    if (0 != pthread_mutex_lock(&db->nonce_lock)) {
        LOG("Could not lock nonce db\n");
        return false;
    }

    /* if the db contains the key, we return an error */
    if (db->nonces->end() != db->nonces->find(key)) {
        LOG("nonces DB contained key %s already, cannot insert\n", key.c_str());
        return false;
    }

    DEBUG("Inserting into nonce DB\n");
    (*db->nonces)[key] = value; /* insert the element */

    if (0 != pthread_mutex_unlock(&db->nonce_lock)) {
        LOG("Could not unlock nonce db\n");
        return false;
    }

    return true;
}

bool db_nonce_contains(db_t *db, char *keyp)
{
    assert(db != NULL);

    std::string key(keyp, NONCE_SIZE);

    return db->nonces->end() != db->nonces->find(key);
}

bool db_nonce_get(db_t *db, char *keyp)
{
    assert(db != NULL);

    std::string key(keyp, NONCE_SIZE);

    return (*db->nonces)[key];

}
bool db_nonce_remove(db_t *db, char *keyp)
{
    assert(db != NULL);

    std::string key(keyp, NONCE_SIZE);

    auto loc = db->nonces->find(key);
    if (db->nonces->end() == loc) {
        LOG("Could not find nonce in DB: %s\n", key.c_str());
        return false;
    }
    db->nonces->erase(loc);
    return true;
}

/* CARD DB */
bool db_card_insert(db_t *db, char *namep, char *cardp)
{
    assert(db != NULL);

    std::string name(namep);
    std::string card(cardp, CARD_SIZE);

    if (0 != pthread_mutex_lock(&db->nonce_lock)) {
        LOG("Could not lock nonce db\n");
        return false;
    }

    /* if the db contains the card, we return an error */
    if (db->cards->end() != db->cards->find(name)) {
        LOG("cards DB contained card for %s already, cannot insert\n", name.c_str());
        return false;
    }

    DEBUG("Inserting into card DB\n");
    (*db->cards)[name] = card; /* insert the element */

    if (0 != pthread_mutex_unlock(&db->nonce_lock)) {
        LOG("Could not unlock nonce db\n");
        return false;
    }

    return true;
}

bool db_card_contains(db_t *db, char *namep)
{
    assert(db != NULL);

    std::string name(namep);

    return db->cards->end() != db->cards->find(name);
}

std::string db_card_get(db_t *db, char *namep)
{
    assert(db != NULL);

    std::string name(namep);

    return (*db->cards)[name];
}
