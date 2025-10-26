#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <openssl/evp.h>

// Correspondence
typedef struct {
    char *raw;
    char algorithm[7];
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;
} row;



/*
 IO
*/

char *read_line(FILE *file) {
    if (!file) {
        return NULL;
    }

    size_t limitWidthChar = 100;
    size_t numChar = 0;

    // Malloc a line of character
    char *buffer = malloc(limitWidthChar);
    if (!buffer) {
        return NULL;
    }

    int character;

    // Read characters in line
    while ((character = fgetc(file)) != EOF) {
        if (character == '\n') {
            break;
        }

        // Check that the string is not too long (without forgetting the '\0').
        if (numChar + 1 >= limitWidthChar) {
            while ((character = fgetc(file)) != EOF && character != '\n') {
            }
            free(buffer);
            return NULL;
        }

        // Add character in line
        buffer[numChar++] = (char)character;
    }

    buffer[numChar] = '\0';

    // Check if there are characters
    if (character == EOF && numChar == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

int read_lines(const char *path, char ***out_lines, size_t *out_count) {
    size_t limit_lines = 1000;
    size_t num_line = 0;

    // Open dictionary
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 1;
    }

    // Malloc a table of lines
    char **buffer_lines = malloc(limit_lines * sizeof(char *));
    if (!buffer_lines) {
        fclose(file);
        return 1;
    }

    // Add line in table
    while (true) {
        if (num_line >= limit_lines) {
            break;
        }

        char *line = read_line(file);

        if (!line) {
            break;
        }

        buffer_lines[num_line++] = line;
    }

    // Close dictionary
    fclose(file);

    *out_lines = buffer_lines;
    *out_count = num_line;
    return 0;
}

int read_stdin_lines(char ***out_lines, size_t *out_count) {
    if (!out_lines || !out_count) {
        return 1;
    }

    const size_t max_lines = 1000;

    // Malloc lines of characters
    char **buffer = malloc(max_lines * sizeof(*buffer));
    if (!buffer) {
        return 1;
    }

    size_t num_lines = 0;
    while (true) {
        if (num_lines >= max_lines) {
            break;
        }
        char *line = read_line(stdin);
        if (!line) {
            break;
        }
        buffer[num_lines++] = line;
    }
    *out_lines = buffer;
    *out_count = num_lines;
    return 0;
}

void free_lines(char **lines, size_t count) {
    if (!lines) {
        return;
    }

    // Free line in table
    for (size_t i = 0; i < count; ++i) {
        free((lines)[i]);
    }

    // Free table
    free(lines);
}



/*
 CSV
*/

int write_csv(const char *path, const row *rows, size_t count) {
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
int load_csv(const char *path, row **out_rows, size_t *out_count) {
    *out_rows = NULL;
    *out_count = 0;

    // Open the file in binary mode
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 1;
    }

    // Open the file in binary mode
    const size_t max_lines = 1000;
    row *rows = calloc(max_lines, sizeof(*rows));
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



/*
 App
*/

int merge_table(const char *algorithm, char **lines, size_t count, row **out_rows, size_t *out_count) {

    // Number of row
    size_t nb_row = 0;

    // Create table of rows
    row *table = calloc(count, sizeof(*table));
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

void free_table(row *rows) {
    free(rows);
}

void free_table_rows(row *rows, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(rows[i].raw);
    }
    free(rows);
}



/*
 Hash
*/

// Conversion of digest bytes to digest hexadecimal
static void bytes_to_hexadecimal(const unsigned char *digest, unsigned int len_digest, char *out_hexadecimal) {
    // Conversion table
    static const char table[] = "0123456789abcdef";
    // Converts bytes to hexadecimal
    for (unsigned int i = 0; i < len_digest; ++i) {
        // 4 first bits
        out_hexadecimal[2*i]   = table[(digest[i] >> 4) & 0xF];
        // 4 second bits
        out_hexadecimal[2*i+1] = table[digest[i] & 0xF];
    }
    out_hexadecimal[2*len_digest] = '\0';
}

// Conversion of digest hexadecimal to digest bytes
static int hexadecimal_to_bytes(const char *hexadecimal, unsigned char *out_digest, unsigned int *out_len) {
    size_t len_hexadecimal = strlen(hexadecimal);
    if (len_hexadecimal == 0) {
        fprintf(stderr, "ERROR: Digest is empty.\n");
        return 1;
    }

    // Converts hexadecimal to bytes
    unsigned int num_bytes = 0;
    for (size_t i = 0; i < len_hexadecimal; i += 2) {
        int first_char_hexa;
        int second_char_hexa;

        // First part of hexadecimal
        int first_char = hexadecimal[i];
        // Convert to hexadecimal
        if (first_char >= '0' && first_char <= '9') {
            first_char_hexa = first_char - '0';
        } else if (first_char >= 'a' && first_char <= 'f') {
            first_char_hexa = first_char - 'a' + 10;
        } else if (first_char >= 'A' && first_char <= 'F') {
            first_char_hexa = first_char - 'A' + 10;
        }

        // Second part of hexadecimal
        int second_char = hexadecimal[i+1];
        // Convert to hexadecimal
        if (second_char >= '0' && second_char <= '9') {
            second_char_hexa = second_char - '0';
        } else if (second_char >= 'a' && second_char <= 'f') {
            second_char_hexa = second_char - 'a' + 10;
        } else if (second_char >= 'A' && second_char <= 'F') {
            second_char_hexa = second_char - 'A' + 10;
        }

        // Merge byte
        out_digest[num_bytes++] = (unsigned char)((first_char_hexa << 4) | second_char_hexa);
    }

    *out_len = num_bytes;
    return 0;
}


// Calculate digest of string
static int calculate_digest(const char *algorithm, const char *line, unsigned char *out_digest, unsigned int *out_digest_len) {
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



/*
 Main
*/

static int lookup_matches(const row *rows, size_t num_rows, const char *hex_query) {
    unsigned char querie_digest[EVP_MAX_MD_SIZE];
    unsigned int querie_len = 0;

    // Convert hexadecimal to binary
    if (hexadecimal_to_bytes(hex_query, querie_digest, &querie_len) != 0) {
        return 1;
    }

    size_t found = 0;
    for (size_t i = 0; i < num_rows; ++i) {
        if (rows[i].digest_len == querie_len &&
            memcmp(rows[i].digest, querie_digest, querie_len) == 0) {
            puts(rows[i].raw);
            ++found;
        }
    }
    return found ? 0 : 2;
}

static void print_help(const char *BASENAME) {
    fprintf(stderr,
            "Usage:\n"
            "  %s -G -i <dictionary.txt> [-a <algorithm>]\n"
            "  %s -L -i <dictionary.txt> [-a <algorithm>]\n"
            "Options:\n"
            "  -h, --help\n"
            "  -G, --generate\n"
            "  -L, --lookup\n"
            "  -i, --input <file.txt>\n"
            "  -a, --algorithm <md5|sha1|sha256>\n",
            BASENAME, BASENAME);
}

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
        char **lines = NULL;
        size_t count = 0;

        // Storing lines in RAM
        if (read_lines(filename, &lines, &count) != 0) {
            fprintf(stderr, "ERROR: Read lines failed.\n");
            return 1;
        }

        // Create the chain-condensate correspondence table
        row *rows = NULL;
        size_t num_rows = 0;
        if (merge_table(algorithm, lines, count, &rows, &num_rows) != 0) {
            fprintf(stderr, "ERROR: Create table failed.\n");
            free_lines(lines, count);
            return 1;
        }

        // Write the chain-condensate correspondence table in a CSV
        if (write_csv("/app/output/t3c.csv", rows, num_rows) != 0) {
            fprintf(stderr, "ERROR: Error writing CSV.\n");
            free_lines(lines, count);
            return 1;
        }

        free_table(rows);
        free_lines(lines, count);
    } else if (mode_lookup) {
        // Load CSV
        row *rows = NULL;
        size_t num_rows = 0;
        if (load_csv(filename, &rows, &num_rows) != 0) {
            fprintf(stderr, "ERROR: Load CSV failed.\n");
            return 1;
        }

        // Read digest (stdin)
        char **queries = NULL;
        size_t querie_count = 0;
        if (read_stdin_lines(&queries, &querie_count) != 0) {
            fprintf(stderr, "ERROR: Load CSV failed.\n");
            free_table_rows(rows, num_rows);
            return 1;
        }

        // Print digest (stdin)
        for (size_t i = 0; i < querie_count && i < 5; ++i) {
            lookup_matches(rows, num_rows, queries[i]);
        }

        free_lines(queries, querie_count);
        free_table_rows(rows, num_rows);
    }

    return 0;
}
