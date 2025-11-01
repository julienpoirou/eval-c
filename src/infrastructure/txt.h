#ifndef HEADER_TXT
#define HEADER_TXT

#include <stdio.h>
#include <stddef.h>

char *read_line(FILE *file);
int read_lines(const char *path, char ***out_lines, size_t *out_count);
void free_lines(char **lines, size_t count);
int read_stdin_lines(char ***out_lines, size_t *out_count);

#endif
