#ifndef HEADER_HEXADECIMAL
#define HEADER_HEXADECIMAL

void bytes_to_hexadecimal(const unsigned char *digest, unsigned int len_digest, char *out_hexadecimal);
int hexadecimal_to_bytes(const char *hexadecimal, unsigned char *out_digest, unsigned int *out_len);

#endif
