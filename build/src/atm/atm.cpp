#include "atm.h"

int atm_main(int argc, char **argv)
{
    int hsock, host_port, c, amt_set, acc_set;
    unsigned transfer_size;
    char *buffer = NULL, *auth_file = NULL, *host_name = NULL,
            *card_file = NULL, *end = NULL, *auth_file_contents = NULL;
    struct transfer *atm_transfer;

    amt_set = 0;
    acc_set = 0;
    host_port = 0;
    transfer_size = sizeof(struct transfer);
    atm_transfer = (struct transfer *) malloc(transfer_size);
    if (atm_transfer == NULL) {
        ERR("[-] Unable to allocate");
        return 255;
    }

    atm_transfer->type = '\0';

    LOG("Hello, ATM!\n");

    while ((c = getopt(argc, argv, "s:i:p:c:a:n:d:g")) != -1) {
        switch (c) {
        case 's':
            auth_file = optarg;
            break;
        case 'i':
            host_name = optarg;
            break;
        case 'p':
            host_port = (int) strtol(optarg, &end, 10);
            if (*end) {
                ERR("[-] Unable to parse host port: %s", end);
                return 255;
            }
            if (host_port < 1024 || host_port > 65535) {
                ERR("[-] Port specified is not in valid range: %d", host_port);
                return 255;
            }
            break;
        case 'c':
            card_file = optarg;
            break;
        case 'a':
            if (strlen(optarg) > 250) {
                ERR("[-] Account name too long: %s\n", optarg);
                return 255;
            }
            strcpy(atm_transfer->name, optarg);
            atm_transfer->name[strlen(optarg)] = '\0';
            acc_set = 1;
            break;
        case 'n':
        case 'd':
        case 'w':
            atm_transfer->type = optopt;
            if (!amt_set) {
                amt_set = 1;
                if (!parse_money(&(atm_transfer->amt), optarg)) {
                    return 255;
                }
            } else {
                ERR("[-] Additional optional command specified");
                return 255;
            }
            break;
        case 'g':
            atm_transfer->type = optopt;
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

    if (auth_file == NULL || card_file == NULL || acc_set == 0) {
        ERR("[-] Did not specify required options: auth-file, card-file, account\n");
        return 255;
    }

    if (host_name == NULL || host_port == 0) {
        ERR("[-] Did not specify a bank to connect to\n");
        return 255;
    }

    auth_file_contents = (char *) malloc(SECURE_SIZE);
    if (auth_file_contents == NULL) {
        ERR("[-] Unable to allocate\n");
        return 255;
    }

    if (!read_from_file(auth_file_contents, SECURE_SIZE, auth_file)) {
        return 255;
    }

    if (atm_transfer->type == 'n') {
        if(access(card_file, F_OK) != -1) {
            ERR("[-] Card already exists, you may not create another account with this card\n");
            return 255;
        } else {
            random_bytes(atm_transfer->card_file, SECURE_SIZE);
            if (!write_to_file(atm_transfer->card_file, SECURE_SIZE, card_file)) {
                return 255;
            }
        }
    } else {
        if (!read_from_file(atm_transfer->card_file, SECURE_SIZE, card_file)) {
            return 255;
        }
    }

    if (!(hsock = atm_connect(host_name, host_port))) {
        return 63;
    }

    buffer = (char *) malloc(transfer_size);
    serialize(buffer, atm_transfer);

    if (!atm_send(hsock, buffer, transfer_size)) {
        return 63;
    }

    atm_close(hsock);

    free(buffer);
    free(atm_transfer);
    free(auth_file_contents);
    return 0;
}
