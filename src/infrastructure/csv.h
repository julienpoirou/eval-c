#ifndef HEADER_CSV
#define HEADER_CSV

#include <stddef.h>
#include "domain/array.h"

int write_csv(const char *path, const array *rows, size_t count);
int load_csv(const char *path, array **out_rows, size_t *out_count);

#endif
