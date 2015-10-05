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

int bank_main(int argc, char **argv)
{
    char c, *auth_file, *end;
    int host_port = BANK_PORT;

    LOG("Hello, Bank!\n");

    while ((c = getopt(argc, argv, "p:s:")) != -1) {
        switch (c) {
        case 's':
            auth_file = optarg;
            break;
        case 'p':
            host_port = (int) strtol(optarg, &end, 10);
            if (*end) {
                ERR("[-] Unable to parse host port: %s", end);
            }
            if (host_port < 1024 || host_port > 65535) {
                ERR("[-] Port specified is not in valid range: %d", host_port);
                return 255;
            }
            break;
        case '?':
            if (optopt == 'c') {
                ERR("[-] Option -%c requires an argument.\n", optopt);
            } else if (isprint(optopt)){
                ERR("[-] Unknown option `-%c'.\n", optopt);
            } else {
                ERR("[-] Unknown option character `\\x%x'.\n", optopt);
            }
            return 255;
        default:
            ERR("[-] Error while parsing options\n");
            return 255;
        }
    }

    bank_create_server();
    return 0;
}
