#ifndef DB_H
#define DB_H
#include "shared/macros.h"
#include "shared/config.h"
#include "shared/money.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
#include <map>
#include <string>

typedef struct db_s {
    pthread_mutex_t balance_lock;
    std::map<std::string, struct db_money> *balances;
    pthread_mutex_t nonce_lock;
    std::map<std::string, bool> *nonces;
} db_t;

/* Create a new DB.
 * Returns NULL if there was an error */
db_t *db_create();

/* Destroy a DB.
 *
 * Returns whether the operation was a success.
 */
bool db_destroy(db_t *db);

/* DB Money ops */
/* Insert a balance into the DB (must be a first-time addition.
 * For changing an existing balance, see db_update).
 *
 * Returns whether the operation was a success
 * */
bool db_insert(db_t *db, std::string key, struct db_money &value);

/* Update an existing key in the database to a new value.
 * For inserting an element into the database, see db_insert
 *
 * Returns whether the operation was a success
 */
bool db_update(db_t *db, std::string key, struct db_money &value);

/* Return whether a db contains a given key
 */
bool db_contains(db_t *db, std::string key);

/* Retrieve a balance associated with a key from the db.
 *
 * If the db does not contain that key, behavior is undefined,
 * so you should call db_contains() first to check.
 *
 * Returns the `struct db_money` associated with the key.
 */
struct db_money db_get(db_t *db, std::string key);

/* DB nonce ops */
/* Insert a nonce into the nonce db
 *
 * Returns whether the operation was a success
 * */
bool db_nonce_insert(db_t *db, std::string key, bool value);

/* Return whether a db contains a given key
 */
bool db_nonce_contains(db_t *db, std::string key);

/* Retrieve a name associated with a key from the nonce db.
 *
 * If the db does not contain that key, behavior is undefined,
 * so you should call db_nonce_contains() first to check.
 *
 * Returns the user associated with the key.
 */
bool db_nonce_get(db_t *db, std::string key);

/* Remove a key from the DB.
 * returns whether the op was a success
 */
bool db_nonce_remove(db_t *db, std::string key);

/* nonce ops overloaded to take usigned char* */
bool db_nonce_insert(db_t *db, unsigned char* keyp, bool value);

bool db_nonce_contains(db_t *db, unsigned char* keyp);

bool db_nonce_get(db_t *db, unsigned char* keyp);

bool db_nonce_remove(db_t *db, unsigned char* keyp);

#endif
