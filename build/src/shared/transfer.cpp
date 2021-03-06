#include "transfer.h"


int serialize(char *dst, struct transfer *src)
{
    unsigned cur_size;
    cur_size = 0;

    dst[0] = src->type;
    cur_size += 1;

    memcpy(dst + cur_size, &(src->nonce), sizeof(src->nonce));
    cur_size += sizeof(src->nonce);

    memcpy(dst + cur_size, src->name, sizeof(src->name));
    cur_size += sizeof(src->name);

    src->amt.dollars = htonl(src->amt.dollars);
    memcpy(dst + cur_size, &(src->amt.dollars), sizeof(src->amt.dollars));
    src->amt.dollars = ntohl(src->amt.dollars);
    cur_size += sizeof(src->amt.dollars);

    memcpy(dst + cur_size, &(src->amt.cents), sizeof(src->amt.cents));
    cur_size += sizeof(src->amt.cents);

    memcpy(dst + cur_size, src->card, sizeof(src->card));
    return 1;
}

int deserialize(struct transfer *dst, char *src)
{
    unsigned cur_size;
    cur_size = 0;

    dst->type = src[0];
    cur_size += 1;

    memcpy(&(dst->nonce), src + cur_size, sizeof(dst->nonce));
    cur_size += sizeof(dst->nonce);

    memcpy(dst->name, src + cur_size, sizeof(dst->name));
    cur_size += sizeof(dst->name);

    memcpy(&(dst->amt.dollars), src + cur_size, sizeof(dst->amt.dollars));
    dst->amt.dollars = ntohl(dst->amt.dollars);
    cur_size += sizeof(dst->amt.dollars);

    memcpy(&(dst->amt.cents), src + cur_size, sizeof(dst->amt.cents));
    cur_size += sizeof(dst->amt.cents);

    memcpy(dst->card, src + cur_size, sizeof(dst->card));
    return 1;
}

int print_transfer(char type, struct transfer *t)
{
    const char *type_message[4] = {"initial_balance", "deposit",
                             "withdraw", "balance"};
    const char *msg;

    msg = NULL;
    switch (type) {
    case 'n':
        msg = type_message[0];
        break;
    case 'd':
        msg = type_message[1];
        break;
    case 'w':
        msg = type_message[2];
        break;
    case 'g':
        msg = type_message[3];
        break;
    default:
        ERR("[-] Invalid type given: %c", type);
        return 0;
    }

    fputs("{\"account\":\"", stdout);
    print_escaped_string(t->name);
    fputs("\",\"", stdout);
    fputs(msg, stdout);
    fputs("\":", stdout);
    print_money(&(t->amt));
    fputs("}\n", stdout);

    fflush(stdout);
    return 1;
}
