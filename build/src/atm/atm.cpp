#include "atm.h"

int atm_main(int argc, char **argv)
{
    int hsock, host_port, get_current_balance, c;
    unsigned buffer_len;
    char *buffer = NULL, *auth_file = NULL, *host_name = NULL,
            *card_file = NULL, *account = NULL, *end = NULL;

    get_current_balance = 0;
    host_port = 0;

    LOG("Hello, ATM!\n");

    /*
    TODO: Check if given parameters are a valid permutation of the spec's expected params
    atm [-s <auth-file>] [-i <ip-address>] [-p <port>] [-c <card-file>] -a <account> -n <balance>
    atm [-s <auth-file>] [-i <ip-address>] [-p <port>] [-c <card-file>] -a <account> -d <amount>
    atm [-s <auth-file>] [-i <ip-address>] [-p <port>] [-c <card-file>] -a <account> -w <amount>
    atm [-s <auth-file>] [-i <ip-address>] [-p <port>] [-c <card-file>] -a <account> -g
    */
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
            account = optarg;
            break;
        case 'n':
            // TODO
            break;
        case 'd':
            // TODO
            break;
        case 'w':
            // TODO
            break;
        case 'g':
            get_current_balance = 1;
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

    if (auth_file == NULL || card_file == NULL || account == NULL) {
        ERR("[-] Did not specify required options: auth-file, card-file, account\n");
        return 255;
    }

    if (host_name == NULL || host_port == 0) {
        ERR("[-] Did not specify a bank to connect to\n");
        return 255;
    }

    if ((hsock = atm_connect(host_name, host_port)) == 63) {
        return 63;
    }

    // Testing message
    const char *test = "LOL HI SERVER :3\x00";
    buffer = (char *) malloc(strlen(test));
    strcpy(buffer, test);
    buffer_len = strlen(buffer);

    if (atm_send(hsock, buffer, buffer_len) == 63) {
        return 63;
    }

    atm_close(hsock);

    return 0;
}
