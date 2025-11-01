#include "application/generate.h"
#include <stdio.h>

#include "domain/array.h"
#include "infrastructure/txt.h"
#include "infrastructure/csv.h"

int generate_to_csv(const char *input_path, const char *algorithm, const char *output_path) {
    char **lines = NULL;
    size_t count = 0;

    // Storing lines in RAM
    if (read_lines(input_path, &lines, &count) != 0) {
        fprintf(stderr, "ERROR: Read lines failed.\n");
        return 1;
    }

    // Create the chain-condensate correspondence table
    array *rows = NULL;
    size_t num_rows = 0;
    if (merge_table(algorithm, lines, count, &rows, &num_rows) != 0) {
        fprintf(stderr, "ERROR: Create table failed.\n");
        free_lines(lines, count);
        return 1;
    }

    // Write the chain-condensate correspondence table in a CSV
    if (write_csv(output_path, rows, num_rows) != 0) {
        fprintf(stderr, "ERROR: Error writing CSV.\n");
        free_lines(lines, count);
        return 1;
    }

    free_table(rows);
    free_lines(lines, count);
    return 0;
}