#include "util.h"

int is_valid_name(char *name)
{
    unsigned i;
    static const char valid_char[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    for (i = 0; i < strlen(name); i++) {
        if (!valid_char[(unsigned) name[i]]) return 0;
    }
    return 1;
}

int is_valid_filename(char *filename)
{
    if (!strcmp(filename, "..") || !strcmp(filename, ".")
        || !is_valid_name(filename)) {
        return 0;
    }
    return 1;
}

void print_escaped_string(char *str)
{
    unsigned i, len;

    len = strlen(str);
    for (i = 0; i < len; i++) {
        if (str[i] == '"' || str[i] == '\'') putc('\\', stdout);
        putc(str[i], stdout);
    }
}

int random_bytes(char* buf, size_t len)
{
    int random_data = open("/dev/urandom", O_RDONLY);
    size_t random_data_read = 0;
    while (random_data_read < len) {
        ssize_t result = read(random_data, buf + random_data_read, len - random_data_read);
        if (result < 0) {
            ERR("Unable to generate random numbers: %d\n", errno);
            return 0;
        }
        random_data_read += result;
    }
    close(random_data);
    return 1;
}

unsigned get_file_size(const char * file_name)
{
    struct stat sb;
    if (stat (file_name, & sb) != 0) {
        ERR("'stat' failed for '%s': %s.\n",
                 file_name, strerror (errno));
        return 0;
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
        return 0;
    }

    s = get_file_size(file_name);
    if (s != read_size) {
        ERR("[-] File size '%d' not equal to expected size '%d'\n", s, read_size);
        return 0;
    }

    f = fopen(file_name, "r");
    if (! f) {
        ERR("[-] Could not open '%s': %s.\n", file_name,
                 strerror(errno));
        return 0;
    }
    bytes_read = fread(dst, sizeof(unsigned char), s, f);
    if (bytes_read != s) {
        ERR("[-] Short read of '%s': expected %d bytes "
                 "but got %d: %s.\n", file_name, s, bytes_read,
                 strerror(errno));
        return 0;
    }
    status = fclose(f);
    if (status != 0) {
        ERR("[-] Error closing '%s': %s.\n", file_name,
                 strerror(errno));
        return 0;
    }

    return 1;
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
        return 0;
    }
    bytes_written = fwrite(src, sizeof(unsigned char), write_size, f);
    if (bytes_written != write_size) {
        ERR("Short write of '%s': expected to write %d bytes "
                 "but got %d: %s.\n", file_name, write_size, bytes_written,
                 strerror(errno));
        return 0;
    }
    status = fclose(f);
    if (status != 0) {
        ERR("Error closing '%s': %s.\n", file_name,
                 strerror(errno));
        return 0;
    }

    return 1;
}

/*
  reads an unknown variable amount of bytes from sock. returns a void pointer 
  to heap allocated data. should be casted to appropriate type on return.
  returns NULL on any error
*/
void* recv_var_bytes(int sock) {

    int bytecount;
    void* buf = malloc(SECURE_SIZE);
    void* tmp;
    if (buf == NULL) {
	ERR("[-] Unable to allocate\n");
	return NULL;
    }
    
    bytecount = 0;
    while (1) {
	int j = recv(sock, (unsigned char*)buf + bytecount, SECURE_SIZE, 0);

	if (j < 0) {
	    ERR("[-] Error receiving data\n");
	    free(buf);
	    return NULL;
	}
	if (j == 0)
	    break;
	
	if (j < SECURE_SIZE) {
	    bytecount += j;
	    continue;
	}

	if (j == SECURE_SIZE) {
	    tmp = (char *) realloc(buf, (bytecount += SECURE_SIZE));
	    if (tmp == NULL) {
		ERR("[-] Unable to allocate\n");
		free(buf);
		return NULL;
	    }
	    else buf = tmp;
	    continue;
	}
    }
    return buf;
}
