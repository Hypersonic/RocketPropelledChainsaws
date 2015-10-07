#include "transfer.h"

int serialize(char *dst, struct transfer *src)
{
    unsigned cur_size;
    cur_size = 0;

    dst[0] = src->type;
    cur_size += 1;

    memcpy(dst + cur_size, src->name, sizeof(src->name));
    cur_size += sizeof(src->name);

    src->amt.dollars = htonl(src->amt.dollars);
    memcpy(dst + cur_size, &(src->amt.dollars), sizeof(src->amt.dollars));
    src->amt.dollars = ntohl(src->amt.dollars);
    cur_size += sizeof(src->amt.dollars);

    memcpy(dst + cur_size, &(src->amt.cents), sizeof(src->amt.cents));
    cur_size += sizeof(src->amt.cents);

    memcpy(dst + cur_size, src->card_file, sizeof(src->card_file));
    return 0;
}

int deserialize(struct transfer *dst, char *src)
{
    unsigned cur_size;
    cur_size = 0;

    dst->type = src[0];
    cur_size += 1;

    memcpy(dst->name, src + cur_size, sizeof(dst->name));
    cur_size += sizeof(dst->name);

    memcpy(&(dst->amt.dollars), src + cur_size, sizeof(dst->amt.dollars));
    dst->amt.dollars = ntohl(dst->amt.dollars);
    cur_size += sizeof(dst->amt.dollars);

    memcpy(&(dst->amt.cents), src + cur_size, sizeof(dst->amt.cents));
    cur_size += sizeof(dst->amt.cents);

    memcpy(dst->card_file, src + cur_size, sizeof(dst->card_file));
    return 0;
}
