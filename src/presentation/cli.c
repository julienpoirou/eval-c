#include "presentation/cli.h"
#include <stdio.h>

void print_help(const char *BASENAME) {
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
