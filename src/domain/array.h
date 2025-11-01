#ifndef HEADER_ARRAY
#define HEADER_ARRAY

#include <stddef.h>
#include <openssl/evp.h>

typedef struct {
    char *raw;
    char algorithm[7];
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;
} array;

int calculate_digest(const char *algorithm, const char *line, unsigned char *out_digest, unsigned int *out_digest_len);
int merge_table(const char *algorithm, char **lines, size_t count, array **out_rows, size_t *out_count);
void free_table(array *rows);
void free_table_rows(array *rows, size_t count);

#endif
