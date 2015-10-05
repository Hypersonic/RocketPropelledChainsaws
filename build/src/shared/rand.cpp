#include "util.h"

int random_bytes(char* buf, size_t len)
{
    int random_data = open("/dev/random", O_RDONLY);
    size_t random_data_read = 0;
    while (random_data_read < len) {
        ssize_t result = read(random_data, buf + random_data_read, len - random_data_read);
        if (result < 0) {
            ERR("Unable to generate random numbers: %d", errno);
            return 255;
        }
        random_data_read += result;
    }
    close(random_data);
    return 0;
}
