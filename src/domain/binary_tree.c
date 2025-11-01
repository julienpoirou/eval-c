#include "domain/binary_tree.h"
#include <stdlib.h>
#include <string.h>

// Compare two digests
static int compare_digest(const array *a, const unsigned char *d, unsigned int len) {
    if (a->digest_len != len) return (a->digest_len < len) ? -1 : 1;
    int c = memcmp(a->digest, d, len);
    return (c < 0) ? -1 : (c > 0 ? 1 : 0);
}

// Insert the pointer in a row in the binary search tree
static int tree_insert(tree **root, const array *row) {
    if (!*root) {
        tree *n = (tree *)calloc(1, sizeof(*n));
        if (!n) return 1;
        n->row = row;
        *root = n;
        return 0;
    }
    int c = compare_digest((*root)->row, row->digest, row->digest_len);
    if (c > 0) return tree_insert(&(*root)->left,  row);
    if (c < 0) return tree_insert(&(*root)->right, row);
    return 0;
}

// Create the binary search tree
int tree_build(const array *rows, size_t num_rows, tree **out_root) {
    *out_root = NULL;
    for (size_t i = 0; i < num_rows; ++i) {
        if (tree_insert(out_root, &rows[i]) != 0) return 1;
    }
    return 0;
}

// Find digest in the binary search tree
const array *tree_find(const tree *root, const unsigned char *digest, unsigned int len) {
    const tree *cur = root;
    while (cur) {
        int c = compare_digest(cur->row, digest, len);
        if (c == 0) return cur->row;
        cur = (c > 0) ? cur->left : cur->right;
    }
    return NULL;
}

void tree_free(tree *root) {
    if (!root) return;
    tree_free(root->left);
    tree_free(root->right);
    free(root);
}
