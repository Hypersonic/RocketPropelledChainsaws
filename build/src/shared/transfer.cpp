#include "transfer.h"

bool serialize(struct transfer *src, char *dst)
{
    src->amt.dollars = htonl(src->amt.dollars);
    memcpy(dst, src, sizeof(struct transfer));
    src->amt.dollars = ntohl(src->amt.dollars);
    return true;
}

bool deserialize(char *src, struct transfer *dst)
{
    memcpy(dst, src, sizeof(struct transfer));
    dst->amt.dollars = ntohl(dst->amt.dollars);
    return true;
}