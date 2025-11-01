#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "bootstrap/main.h"
#include "presentation/cli.h"
#include "application/lookup.h"
#include "application/generate.h"
#include "domain/array.h"
#include "domain/binary_tree.h"
#include "infrastructure/txt.h"
#include "infrastructure/csv.h"

int main(int argc, char *argv[]) {
    const char *BASENAME = argv[0];
    bool mode_generate = false;
    bool mode_lookup = false;
    const char *filename = NULL;
    const char *algorithm = "md5";

    if (argc < 2) {
        print_help(BASENAME);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-G") == 0 || strcmp(argv[i], "--generate") == 0) {
            mode_generate = true;
        } else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--lookup") == 0) {
            mode_lookup = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(BASENAME);
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 < argc) {
                filename = argv[++i];
                FILE *file = fopen(filename, "r");
                if (file == NULL) {
                    fprintf(stderr, "ERROR: The file does not exist.\n");
                    return 1;
                } else {
                    fclose(file);
                }
            } else {
                fprintf(stderr, "ERROR: Filename not specified.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
            if (i + 1 < argc) {
                algorithm = argv[++i];
                if (!(strcmp(algorithm, "md5") == 0 ||
                      strcmp(algorithm, "sha1") == 0 ||
                      strcmp(algorithm, "sha256") == 0)) {
                    fprintf(stderr, "ERROR: Supported algorithms (md5, sha1, sha256).\n");
                    return 1;
                }
            } else {
                fprintf(stderr, "ERROR: Algorithm not specified (md5, sha1, sha256).\n");
                return 1;
            }
        } else {
            fprintf(stderr, "ERROR: Unknown options.\n");
            return 1;
        }
    }

    if (!(mode_generate || mode_lookup)) {
        fprintf(stderr, "ERROR: -G or -L is required.\n");
        return 1;
    } else if (mode_generate && mode_lookup) {
        fprintf(stderr, "ERROR: -G and -L can't be used together.\n");
        return 1;
    }

    if (filename == NULL) {
        fprintf(stderr, "ERROR: Filename is not specified.\n");
        return 1;
    }

    if (mode_generate) {
        if (generate_to_csv(filename, algorithm, "/app/output/t3c.csv") != 0) {
            return 1;
        }
    } else if (mode_lookup) {
        // Load CSV
        array *rows = NULL;
        size_t num_rows = 0;
        if (load_csv(filename, &rows, &num_rows) != 0) {
            fprintf(stderr, "ERROR: Load CSV failed.\n");
            return 1;
        }

        // Binary tree
        tree *index = NULL;
        if (tree_build(rows, num_rows, &index) != 0) {
            fprintf(stderr, "ERROR: Build binary search tree.\n");
            free_table_rows(rows, num_rows);
            return 1;
        }

        // Read digest (stdin)
        while (true) {
            char *line = read_line(stdin);
            if (!line) {
                break;
            }
            lookup_matches(index, line);

            free(line);
        }

        tree_free(index);
        free_table_rows(rows, num_rows);
    }

    return 0;
}
