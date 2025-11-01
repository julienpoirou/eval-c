#include "infrastructure/hexadecimal.h"
#include <stdio.h>
#include <string.h>

// Conversion of digest bytes to digest hexadecimal
void bytes_to_hexadecimal(const unsigned char *digest, unsigned int len_digest, char *out_hexadecimal) {
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
int hexadecimal_to_bytes(const char *hexadecimal, unsigned char *out_digest, unsigned int *out_len) {
    size_t len_hexadecimal = strlen(hexadecimal);
    if (len_hexadecimal == 0) {
        fprintf(stderr, "ERROR: Digest is empty.\n");
        return 1;
    }

    // Converts hexadecimal to bytes
    unsigned int num_bytes = 0;
    for (size_t i = 0; i < len_hexadecimal; i += 2) {
        int first_char_hexa = 0;
        int second_char_hexa = 0;

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
