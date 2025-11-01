#include "infrastructure/csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/evp.h>

#include "infrastructure/txt.h"
#include "infrastructure/hexadecimal.h"

int write_csv(const char *path, const array *rows, size_t count) {
    if (!rows) {
        return 1;
    }

    // Open CSV
    FILE *out_csv = fopen(path, "wb");
    if (!out_csv) {
        fprintf(stderr, "ERROR: The directory does not exist.\n");
        return 1;
    }

    // Write header in CSV
    fputs("raw,algorithm,digest\n", out_csv);

    // Write informations
    char hexadecimal[2*EVP_MAX_MD_SIZE+1];
    for (size_t i = 0; i < count; ++i) {
        bytes_to_hexadecimal(rows[i].digest, rows[i].digest_len, hexadecimal);
        fprintf(out_csv, "%s,%s,%s\n", rows[i].raw, rows[i].algorithm, hexadecimal);
    }

    // Close CSV
    fclose(out_csv);
    return 0;
}

int load_csv(const char *path, array **out_rows, size_t *out_count) {
    *out_rows = NULL;
    *out_count = 0;

    // Open the file in binary mode
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 1;
    }

    // Open the file in binary mode
    const size_t max_lines = 1000;
    array *rows = calloc(max_lines, sizeof(*rows));
    if (!rows) {
        fclose(file);
        return 1;
    }

    // Remove header
    char *first_line = read_line(file);
    if (first_line) {
        free(first_line);
    }

    size_t num_lines = 0;

    while(true) {
        if (num_lines >= max_lines) {
            break;
        }

        char *line = read_line(file);
        // EOF or error in line
        if (!line) {
            break;
        }

        // Position of ',' on the line
        char *first_comma = strchr(line, ',');
        if (!first_comma) {
            free(line);
            continue;
        }
        char *second_comma = strchr(first_comma + 1, ',');
        if (!second_comma) {
            free(line);
            continue;
        }

        // Cut into several lines and get the values
        *first_comma = '\0';
        *second_comma = '\0';
        char *raw_str = line;
        char *algorithm = first_comma + 1;
        char *hexadecimal = second_comma + 1;

        // Allocate memory to the raw string
        size_t raw_len = strlen(raw_str);
        rows[num_lines].raw = malloc(raw_len + 1);
        if (!rows[num_lines].raw) {
            free(line);
            free_table_rows(rows, num_lines);
            fclose(file);
            return 1;
        }
        memcpy(rows[num_lines].raw, raw_str, raw_len + 1);

        // Push algorithm
        size_t j = 0;
        while (j + 1 < sizeof(rows[num_lines].algorithm) && algorithm[j] != '\0') {
            rows[num_lines].algorithm[j] = algorithm[j];
            ++j;
        }
        rows[num_lines].algorithm[j] = '\0';

        // Push digest bytes
        unsigned int digest_len = 0;
        if (hexadecimal_to_bytes(hexadecimal, rows[num_lines].digest, &digest_len) != 0) {
            free(line);
            free_table_rows(rows, num_lines);
            fclose(file);
            return 1;
        }
        rows[num_lines].digest_len = digest_len;

        free(line);
        num_lines++;
    }

    fclose(file);
    *out_rows = rows;
    *out_count = num_lines;
    return 0;
}
