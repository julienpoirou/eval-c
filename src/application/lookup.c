#include "application/lookup.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "infrastructure/hexadecimal.h"

int lookup_matches(const tree *root, const char *hex_query) {
    unsigned char querie_digest[EVP_MAX_MD_SIZE];
    unsigned int querie_len = 0;

    // Convert hexadecimal to binary
    if (hexadecimal_to_bytes(hex_query, querie_digest, &querie_len) != 0) {
        return 1;
    }

    const array *found = tree_find(root, querie_digest, querie_len);
    if (found) {
        puts(found->raw);
        return 0;
    }

    return 2;
}