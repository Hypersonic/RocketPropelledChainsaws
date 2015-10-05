#ifndef TRANSFER_H
#define TRANSFER_H

#include <arpa/inet.h>
#include <string>
#include "shared/money.h"

struct transfer {
    uint8_t type;
    char name[251];
    struct money amt;
    char cardfile[256];
};

/* Serializes a transfer struct into a char pointer to be transferred
 * over the network, changing byteorder as needed.
 * Returns whether the serialization was a success
 */
bool serialize(struct transfer *src, char *dst);

/* Deserializes a char pointer into a transfer struct.
 * Returns whether the deserialization was a success
 */
bool deserialize(char *src, struct transfer *dst);

#endif