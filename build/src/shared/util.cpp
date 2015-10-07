#include "util.h"

int random_bytes(char* buf, size_t len)
{
    int random_data = open("/dev/random", O_RDONLY);
    size_t random_data_read = 0;
    while (random_data_read < len) {
        ssize_t result = read(random_data, buf + random_data_read, len - random_data_read);
        if (result < 0) {
            ERR("Unable to generate random numbers: %d\n", errno);
            return 255;
        }
        random_data_read += result;
    }
    close(random_data);
    return 0;
}

unsigned get_file_size(const char * file_name)
{
    struct stat sb;
    if (stat (file_name, & sb) != 0) {
        ERR("'stat' failed for '%s': %s.\n",
                 file_name, strerror (errno));
        return 255;
    }
    return sb.st_size;
}

int read_from_file(char *dst, unsigned read_size, char *file_name)
{
    unsigned s;
    FILE *f;
    unsigned bytes_read;
    int status;

    if(access(file_name, F_OK) == -1) {
        ERR("[-] File does not exist: %s\n", file_name);
        return 255;
    }

    if ((s = get_file_size(file_name)) == 255) {
        return 255;
    }

    if (s != read_size) {
        ERR("[-] File size '%d' not equal to expected size '%d'\n", s, read_size);
        return 255;
    }

    f = fopen(file_name, "r");
    if (! f) {
        ERR("[-] Could not open '%s': %s.\n", file_name,
                 strerror(errno));
        return 255;
    }
    bytes_read = fread(dst, sizeof(unsigned char), s, f);
    if (bytes_read != s) {
        ERR("[-] Short read of '%s': expected %d bytes "
                 "but got %d: %s.\n", file_name, s, bytes_read,
                 strerror(errno));
        return 255;
    }
    status = fclose(f);
    if (status != 0) {
        ERR("[-] Error closing '%s': %s.\n", file_name,
                 strerror(errno));
        return 255;
    }

    return 0;
}

int write_to_file(char *src, unsigned write_size, char *file_name)
{
    FILE *f;
    unsigned bytes_written;
    int status;

    f = fopen(file_name, "w");
    if (! f) {
        ERR("Could not open '%s': %s.\n", file_name,
                 strerror(errno));
        return 255;
    }
    bytes_written = fwrite(src, sizeof(unsigned char), write_size, f);
    if (bytes_written != write_size) {
        ERR("Short write of '%s': expected to write %d bytes "
                 "but got %d: %s.\n", file_name, write_size, bytes_written,
                 strerror(errno));
        return 255;
    }
    status = fclose(f);
    if (status != 0) {
        ERR("Error closing '%s': %s.\n", file_name,
                 strerror(errno));
        return 255;
    }

    return 0;
}
