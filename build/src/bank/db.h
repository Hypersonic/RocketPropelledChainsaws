#ifndef DB_H
#define DB_H
#include "shared/macros.h"
#include "shared/money.h"
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <map>

typedef struct db_s {
    pthread_mutex_t lock;
    std::map<std::string, struct money> balances;
} db_t;

/* Create a new DB.
 * Returns NULL if there was an error */
db_t *db_create();

/* Insert a balance into the DB (must be a first-time addition.
 * For changing an existing balance, see db_update).
 *
 * Returns whether the operation was a success
 * */
bool db_insert(db_t *db, std::string key, struct money value);

/* Update an existing key in the database to a new value.
 * For inserting an element into the database, see db_insert
 *
 * Returns whether the operation was a success
 */
bool db_update(db_t *db, std::string key, struct money value);

#endif