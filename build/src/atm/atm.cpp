#include "atm.h"

int atm_main(int argc, char **argv)
{
    int hsock, host_port, c, amt_set, acc_set, free_card, trans_set;
    unsigned transfer_size, client_ret;
    char *auth_file = NULL, *host_name = NULL,
            *card_file = NULL, *end = NULL, *auth_file_contents = NULL;
    struct transfer *atm_transfer;

    amt_set = 0;
    acc_set = 0;
    trans_set = 0;
    host_port = BANK_PORT;
    host_name = (char *) "127.0.0.1";
    transfer_size = sizeof(struct transfer);
    atm_transfer = (struct transfer *) malloc(transfer_size);
    if (atm_transfer == NULL) {
        ERR("[-] Unable to allocate");
        return 255;
    }

    atm_transfer->type = '\0';

    LOG("Hello, ATM!\n");

    while ((c = getopt(argc, argv, "s:i:p:c:a:n:d:w:g")) != -1) {
        switch (c) {
        case 's':
            if (!is_valid_filename(optarg)) {
                ERR("[-] Invalid account filename: %s\n", optarg);
                return 255;
            }
            auth_file = optarg;
            break;
        case 'i':
            host_name = optarg;
            if ('0' == host_name[0]) {
                ERR("[-] Invalid hostname given\n");
                return 255;
            }
            break;
        case 'p':
            host_port = (int) strtol(optarg, &end, 10);
            if (optarg[0] == '0' || *end) {
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
            free_card = 0;
            break;
        case 'a':
            if (strlen(optarg) < 1 || strlen(optarg) > 250) {
                ERR("[-] Account name too long: %s\n", optarg);
                return 255;
            }
            if (!is_valid_name(optarg)) {
                ERR("[-] Invalid account name: %s\n", optarg);
                return 255;
            }
            strcpy(atm_transfer->name, optarg);
            atm_transfer->name[strlen(optarg)] = '\0';
            acc_set = 1;
            break;
        case 'n':
        case 'd':
        case 'w':
            if (!trans_set) {
                trans_set = 1;
            } else {
                ERR("[-] Command option already specified\n");
                return 255;
            }
            atm_transfer->type = c;
            if (!amt_set) {
                amt_set = 1;
                if (!parse_money(&(atm_transfer->amt), optarg)) {
                    return 255;
                }
            } else {
                ERR("[-] Additional optional command specified\n");
                return 255;
            }
            break;
        case 'g':
            atm_transfer->type = c;
            if (!trans_set) {
                trans_set = 1;
            } else {
                ERR("[-] Command option already specified\n");
                return 255;
            }

            if (optind != argc && argv[optind][0] != '-') {
                ERR("[-] Cannot specify option for option -g\n");
                return 255;
            }
            break;
        case '?':
            if (isprint(optopt)){
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

    if (card_file == NULL && acc_set) {
        if ((card_file = (char *) malloc(strlen(atm_transfer->name) + 6)) == NULL) {
            ERR("[-] Unable to allocate\n");
            return 255;
        }
        strcpy(card_file, atm_transfer->name);
        strcat(card_file, (char *) ".card");
        free_card = 1;
    }

    if (acc_set == 0) {
        ERR("[-] Did not specify name\n");
        return 255;
    }

    if (trans_set == 0) {
        ERR("[-] Did not specify option\n");
        return 255;
    }

    if (host_name == NULL || host_port == 0) {
        ERR("[-] Did not specify a bank to connect to\n");
        return 255;
    }

    auth_file_contents = (char *) malloc(KEY_SIZE);
    if (auth_file_contents == NULL) {
        ERR("[-] Unable to allocate\n");
        return 255;
    }

    if (!read_from_file(auth_file_contents, KEY_SIZE, auth_file)) {
        return 255;
    }

    if (atm_transfer->type == 'n') {
        if (atm_transfer->amt.dollars < 10) {
            ERR("[-] Must deposit 10 or more dollars\n");
            return 255;
        }
        if (access(card_file, F_OK) != -1) {
            ERR("[-] Card already exists, you may not create another account with this card\n");
            return 255;
        } else {
            random_bytes(atm_transfer->card, CARD_SIZE);
            if (!write_to_file(atm_transfer->card, CARD_SIZE, card_file)) {
                return 255;
            }
        }
    } else {
        if (!read_from_file(atm_transfer->card, CARD_SIZE, card_file)) {
            return 255;
        }
    }

    if (!(hsock = atm_connect(host_name, host_port))) {
        if (atm_transfer->type == 'n') {
            if (remove(card_file) != 0) {
                ERR("[-] Unable to remove card file: %s", card_file);
            }
        }
        return 255;
    }

    client_ret = atm_send(hsock, atm_transfer, auth_file_contents);
    if (client_ret == 63 || client_ret == 255) {
        if (atm_transfer->type == 'n') {
            if (remove(card_file) != 0) {
                ERR("[-] Unable to remove card file: %s", card_file);
                return 255;
            }
        }
    	LOG("%d\n", client_ret);
        return client_ret;
    }

    atm_close(hsock);

    if (free_card) {
        free(card_file);
    }
    free(atm_transfer);
    free(auth_file_contents);
    return 0;
}
