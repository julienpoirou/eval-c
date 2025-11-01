#ifndef HEADER_BINARY_TREE
#define HEADER_BINARY_TREE

#include <stddef.h>
#include "domain/array.h"

typedef struct tree {
    const array *row;
    struct tree *left;
    struct tree *right;
} tree;

int tree_build(const array *rows, size_t num_rows, tree **out_root);
const array *tree_find(const tree *root, const unsigned char *digest, unsigned int len);
void tree_free(tree *root);

#endif
