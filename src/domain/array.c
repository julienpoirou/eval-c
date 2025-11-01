#include "domain/array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>


// Calculate digest of string
int calculate_digest(const char *algorithm, const char *line, unsigned char *out_digest, unsigned int *out_digest_len) {
    // Get structure of the algorithm
    const EVP_MD *algorithm_struct = EVP_get_digestbyname(algorithm);
    if (!algorithm_struct) {
        fprintf(stderr, "ERROR: Unsupported algorithms.\n");
        return 1;
    }

    // Get structure of the algorithm
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (!context) {
        fprintf(stderr, "ERROR: Contexte failed.\n");
        return 1;
    }

    // Init context with algorithm
    if (EVP_DigestInit_ex(context, algorithm_struct, NULL) != 1) {
        fprintf(stderr, "ERROR: Adding the algorithm to context failed.\n");
        EVP_MD_CTX_free(context);
        return 1;
    }

    // Add string to hash
    if (EVP_DigestUpdate(context, line, strlen(line)) != 1) {
        fprintf(stderr, "ERROR: Adding the string to context failed.\n");
        EVP_MD_CTX_free(context);
        return 1;
    }

    // Hash the string
    if (EVP_DigestFinal_ex(context, out_digest, out_digest_len) != 1) {
        fprintf(stderr, "ERROR: Hash failed.\n");
        EVP_MD_CTX_free(context);
        return 1;
    }

    EVP_MD_CTX_free(context);
    return 0;
}

int merge_table(const char *algorithm, char **lines, size_t count, array **out_rows, size_t *out_count) {

    // Number of row
    size_t nb_row = 0;

    // Create table of rows
    array *table = calloc(count, sizeof(*table));
    if (!table) {
        return 1;
    }

    // Create table of rows
    for (size_t i = 0; i < count; ++i) {
        // Get raw for the structure
        char *line = lines[i];
        if (!line) {
            continue;
        }

        // Push raw
        table[nb_row].raw = line;

        // Push algorithm
        size_t j = 0;
        while (j + 1 < sizeof(table[nb_row].algorithm) && algorithm[j] != '\0') {
            table[nb_row].algorithm[j] = algorithm[j];
            ++j;
        }
        table[nb_row].algorithm[j] = '\0';

        // Calculate and push digest
        if (calculate_digest(algorithm, line, table[nb_row].digest, &table[nb_row].digest_len) != 0) {
            free(table);
            return 1;
        }

        nb_row++;
    }

    *out_rows = table;
    *out_count = nb_row;
    return 0;
}

void free_table(array *rows) {
    free(rows);
}

void free_table_rows(array *rows, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(rows[i].raw);
    }
    free(rows);
}
