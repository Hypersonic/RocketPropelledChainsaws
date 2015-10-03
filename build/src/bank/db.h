#ifndef DB_H
#define DB_H
#include "shared/macros.h"
#include "shared/money.h"
#include <pthread.h>
#include <map>

typedef struct db_s {
    posix_mutex_t lock;
    std::map<std::string, struct money> balances;
} db_t;

#endif
