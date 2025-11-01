#ifndef HEADER_LOOKUP
#define HEADER_LOOKUP

#include <stddef.h>
#include "domain/array.h"
#include "domain/binary_tree.h"

int lookup_matches(const tree *root, const char *hex_query);

#endif
