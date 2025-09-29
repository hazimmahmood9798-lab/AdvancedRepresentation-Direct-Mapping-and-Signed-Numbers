// convert.c - Assignment 2: Advanced Data Representation (CS 3503)
// Author: Hazim Mahmood
// Description:
//   Implements direct-mapping conversions and signed representations:
//     - oct_to_bin: map each oct digit -> 3 binary bits
//     - oct_to_hex: oct -> bin -> group by 4 -> HEX (uppercase, trim leading zeros except zero)
//     - hex_to_bin: map each hex digit -> 4 binary bits (case-insensitive)
//     - to_sign_magnitude: 32-bit sign-magnitude string
//     - to_ones_complement: 32-bit one's complement string
//     - to_twos_complement: 32-bit two's complement string
//
//   All signed outputs are fixed 32-bit binary strings.
//   Mapping outputs follow the assignment tests exactly.
//
// Notes:
//   - No dynamic allocation; caller provides output buffers.
//   - For mapping functions, inputs are treated as valid digits in the given base.
//
// Compile with main.c (test runner) per README.
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "convert.h"

// --- helpers ---

static void to_32bit_binary(uint32_t value, char *out) {
    // out must have space for 33 chars including null
    for (int i = 31; i >= 0; --i) {
        out[31 - i] = ((value >> i) & 1u) ? '1' : '0';
    }
    out[32] = '\0';
}

static char hex_from_nibble(unsigned nib) {
    static const char *HEX = "0123456789ABCDEF";
    return HEX[nib & 0xF];
}

static unsigned nibble_from_hex(char c) {
    if (c >= '0' && c <= '9') return (unsigned)(c - '0');
    c = (char)toupper((unsigned char)c);
    if (c >= 'A' && c <= 'F') return (unsigned)(10 + (c - 'A'));
    // Treat invalid as 0 
    return 0;
}

// --- Part 1: Direct Mapping Functions ---

void oct_to_bin(const char *oct, char *out) {
    // Map each oct digit to exactly 3 bits (including leading zeros per digit)
    static const char *map[8] = {
        "000","001","010","011","100","101","110","111"
    };
    size_t pos = 0;
    for (size_t i = 0; oct[i] != '\0'; ++i) {
        char c = oct[i];
        if (c >= '0' && c <= '7') {
            const char *m = map[c - '0'];
            out[pos++] = m[0];
            out[pos++] = m[1];
            out[pos++] = m[2];
        } else {
            // ignore non-octal (not expected in tests/spec)
        }
    }
    out[pos] = '\0';
}

void hex_to_bin(const char *hex, char *out) {
    // Map each hex digit to exactly 4 bits (including leading zeros per digit)
    static const char *map[16] = {
        "0000","0001","0010","0011",
        "0100","0101","0110","0111",
        "1000","1001","1010","1011",
        "1100","1101","1110","1111"
    };
    size_t pos = 0;
    for (size_t i = 0; hex[i] != '\0'; ++i) {
        unsigned nib = nibble_from_hex(hex[i]);
        const char *m = map[nib & 0xF];
        out[pos++] = m[0];
        out[pos++] = m[1];
        out[pos++] = m[2];
        out[pos++] = m[3];
    }
    out[pos] = '\0';
}

void oct_to_hex(const char *oct, char *out) {
    // Strategy: oct -> bin (3 bits per digit), then left-pad to multiple of 4,
    // then convert each nibble to uppercase hex. Finally, trim leading zeros,
    // but keep "0" if value is zero.
    char bin[4096]; // generous buffer
    oct_to_bin(oct, bin);

    size_t len = strlen(bin);
    if (len == 0) { strcpy(out, "0"); return; }

    // Left pad to multiple of 4
    size_t rem = len % 4;
    size_t pad = (rem == 0) ? 0 : (4 - rem);

    char padded[4096];
    size_t p = 0;
    for (size_t i = 0; i < pad; ++i) padded[p++] = '0';
    for (size_t i = 0; i < len; ++i)  padded[p++] = bin[i];
    padded[p] = '\0';

    // Convert each nibble
    char hexbuf[4096];
    size_t h = 0;
    for (size_t i = 0; i < p; i += 4) {
        unsigned nib = 0;
        nib |= (padded[i]   == '1') ? 8 : 0;
        nib |= (padded[i+1] == '1') ? 4 : 0;
        nib |= (padded[i+2] == '1') ? 2 : 0;
        nib |= (padded[i+3] == '1') ? 1 : 0;
        hexbuf[h++] = hex_from_nibble(nib);
    }
    hexbuf[h] = '\0';

    // Trim leading zeros, but leave at least one digit
    size_t first_nonzero = 0;
    while (hexbuf[first_nonzero] == '0' && first_nonzero + 1 < h) first_nonzero++;

    strcpy(out, hexbuf + first_nonzero);
}

// --- Part 2: Signed Number Representations ---

void to_sign_magnitude(int32_t n, char *out) {
    // Positive: 32-bit binary as-is
    // Negative: MSB=1, remaining 31 bits = magnitude of |n|
    if (n >= 0) {
        to_32bit_binary((uint32_t)n, out);
        out[0] = '0'; // ensure sign bit 0
        return;
    }
    // Handle negative. For INT_MIN the magnitude does not fit in 31 bits.
    // The assignment tests don't exercise INT_MIN for sign-magnitude;
    // but we'll clamp by using unsigned magnitude of -n except INT_MIN case.
    uint32_t mag;
    if (n == INT32_MIN) {
        // 0x80000000 cannot be represented in 31 magnitude bits in sign-magnitude.
        // We'll set sign bit and leave magnitude zero 
        mag = 0;
    } else {
        mag = (uint32_t)(-n);
    }
    to_32bit_binary(mag, out);
    out[0] = '1'; // set sign bit
}

void to_ones_complement(int32_t n, char *out) {
    // Positive: print as 32-bit
    // Negative: flip all bits of the 32-bit magnitude representation
    if (n >= 0) {
        to_32bit_binary((uint32_t)n, out);
        return;
    }
    // Build 32-bit for |n| then flip
    uint32_t mag = (n == INT32_MIN) ? (uint32_t)INT32_MAX + 1u : (uint32_t)(-n);
    char tmp[33];
    to_32bit_binary(mag, tmp);
    for (int i = 0; i < 32; ++i) {
        out[i] = (tmp[i] == '1') ? '0' : '1';
    }
    out[32] = '\0';
}

void to_twos_complement(int32_t n, char *out) {
    // In C, the in-memory bit pattern of a signed int in two's complement
    // is obtained by casting to uint32_t.
    uint32_t bits = (uint32_t)n;
    to_32bit_binary(bits, out);
}
