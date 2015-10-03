#ifndef DB_H
#define DB_H
#include "shared/macros.h"
#include "shared/money.h"
#include <stdlib.h>
#include <pthread.h>
#include <map>

typedef struct db_s {
    pthread_mutex_t lock;
    std::map<std::string, struct money> balances;
} db_t;

/* Create a new DB.
 * Returns NULL if there was an error */
db_t *db_create();

#endif
