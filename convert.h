// convert.h - header for Assignment 2 conversions
#ifndef CONVERT_H
#define CONVERT_H

#include <stdint.h>

// Part 1
void oct_to_bin(const char *oct, char *out);
void oct_to_hex(const char *oct, char *out);
void hex_to_bin(const char *hex, char *out);

// Part 2
void to_sign_magnitude(int32_t n, char *out);
void to_ones_complement(int32_t n, char *out);
void to_twos_complement(int32_t n, char *out);

#endif // CONVERT_H
