#include "bank.h"

void signal_handler(RELEASE_UNUSED int signal) {
    LOG("[*] Exiting server with signal: %d\n", signal);
    exit(0);
}

int bank_main(int argc, char **argv)
{
    char c, *auth_file, *end, *auth_file_contents;
    int host_port = BANK_PORT;

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    auth_file_contents = NULL;
    auth_file = NULL;

    while ((c = getopt(argc, argv, "p:s:")) != -1) {
        switch (c) {
        case 's':
            if (!is_valid_filename(optarg)) {
                ERR("[-] Invalid file name: %s\n", optarg);
                return 255;
            }
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
            } else if (isprint(optopt)) {
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

    if (auth_file == NULL) {
        auth_file = (char *) "bank.auth";
    }

    if (access(auth_file, F_OK) != -1) {
        ERR("[-] Auth file already exists!\n");
        return 255;
    } else {
        auth_file_contents = (char *) malloc(KEY_SIZE);
        if (auth_file_contents == NULL) {
            ERR("[-] Unable to allocate\n");
            return 255;
        }

        random_bytes(auth_file_contents, KEY_SIZE);
        if (!write_to_file(auth_file_contents, KEY_SIZE, auth_file)) {
            return 255;
        }
        puts("created");
        fflush(stdout);
    }

    if (!bank_create_server(host_port, (unsigned char *) auth_file_contents)) {
        return 255;
    }
    return 0;
}
