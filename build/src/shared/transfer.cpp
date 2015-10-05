#include "transfer.h"

bool serialize(char *dst, struct transfer *src)
{
    src->amt.dollars = htonl(src->amt.dollars);
    memcpy(dst, src, sizeof(struct transfer));
    src->amt.dollars = ntohl(src->amt.dollars);
    return true;
}

bool deserialize(struct transfer *dst, char *src)
{
    memcpy(dst, src, sizeof(struct transfer));
    dst->amt.dollars = ntohl(dst->amt.dollars);
    return true;
}