#include "infrastructure/txt.h"
#include <stdlib.h>
#include <stdbool.h>

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
        if (character == '\r') {
            continue;
        }
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
