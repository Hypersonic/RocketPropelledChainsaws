#include "util.h"

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

unsigned char *read_file(const char *file_name)
{
    unsigned s;
    unsigned char * contents;
    FILE *f;
    size_t bytes_read;
    int status;

    if ((s = get_file_size (file_name)) == 255) {
        return NULL;
    }
    contents = (unsigned char *) malloc(s + 1);
    if (!contents) {
        ERR("Not enough memory.\n");
        return NULL;
    }

    f = fopen (file_name, "r");
    if (! f) {
        ERR("Could not open '%s': %s.\n", file_name,
                 strerror (errno));
        return NULL;
    }
    bytes_read = fread (contents, sizeof (unsigned char), s, f);
    if (bytes_read != s) {
        ERR("Short read of '%s': expected %d bytes "
                 "but got %d: %s.\n", file_name, s, bytes_read,
                 strerror (errno));
        return NULL;
    }
    status = fclose (f);
    if (status != 0) {
        ERR("Error closing '%s': %s.\n", file_name,
                 strerror (errno));
        return NULL;
    }
    return contents;
}
