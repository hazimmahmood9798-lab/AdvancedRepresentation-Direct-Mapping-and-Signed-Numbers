// main.c - Assignment 2 test runner 
// Reads test cases from a text file (default: a2_test.txt) with lines:
//   function_name input [input2] expected_output
// Lines beginning with '#' and blank lines are ignored.
//
// Example output (per spec):
//   Test 1: oct_to_bin("725") -> Expected: "111010101", Got: "111010101" [PASS]
//   Summary: 51/51 tests passed
//
// Build: gcc -Wall -Wextra -O2 -o run main.c convert.c
// Usage: ./run [path/to/testfile]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "convert.h"

#define LINE_MAX 4096

typedef struct {
    char function_name[64];
    char input1[1024];
    char input2[1024];   
    char expected[2048];
    int has_input2;
} TestCase;

static int parse_line(const char *line, TestCase *tc) {
    // skip comments/blank
    const char *p = line;
    while (*p && isspace((unsigned char)*p)) ++p;
    if (*p == '\0' || *p == '#') return 0;

    // Tokenize: function, input1, expected
    // We assume inputs/expected have no spaces (per provided tests)
    char buf[LINE_MAX];
    strncpy(buf, p, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    // Split into tokens
    char *tok = NULL;
    char *save = NULL;
    tok = strtok_r(buf, " \t\r\n", &save);
    if (!tok) return 0;
    strncpy(tc->function_name, tok, sizeof(tc->function_name)-1);
    tc->function_name[sizeof(tc->function_name)-1] = '\0';

    tok = strtok_r(NULL, " \t\r\n", &save);
    if (!tok) return 0;
    strncpy(tc->input1, tok, sizeof(tc->input1)-1);
    tc->input1[sizeof(tc->input1)-1] = '\0';

    // Optional input2 
    tok = strtok_r(NULL, " \t\r\n", &save);
    if (!tok) return 0;
    // Peek ahead to see if there are two inputs (i.e., function needing two args)
    // For A2, we directly assume this is the expected, unless a 4th token exists.
    char third[2048];
    strncpy(third, tok, sizeof(third)-1);
    third[sizeof(third)-1] = '\0';

    tok = strtok_r(NULL, " \t\r\n", &save);
    if (tok) {
        // We have input2 and expected
        strncpy(tc->input2, third, sizeof(tc->input2)-1);
        tc->input2[sizeof(tc->input2)-1] = '\0';
        strncpy(tc->expected, tok, sizeof(tc->expected)-1);
        tc->expected[sizeof(tc->expected)-1] = '\0';
        tc->has_input2 = 1;
    } else {
        // Only expected
        tc->has_input2 = 0;
        strncpy(tc->expected, third, sizeof(tc->expected)-1);
        tc->expected[sizeof(tc->expected)-1] = '\0';
    }
    return 1;
}

int main(int argc, char **argv) {
    const char *path = (argc >= 2) ? argv[1] : "a2_test.txt";
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open test file: %s\n", path);
        return 1;
    }

    char line[LINE_MAX];
    int test_id = 0;
    int passed = 0;
    int total = 0;

    while (fgets(line, sizeof(line), f)) {
        TestCase tc;
        if (!parse_line(line, &tc)) continue;

        ++test_id;
        ++total;

        char actual[4096] = {0};
        int is_pass = 0;

        if (strcmp(tc.function_name, "oct_to_bin") == 0) {
            oct_to_bin(tc.input1, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: oct_to_bin(\"%s\") -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else if (strcmp(tc.function_name, "oct_to_hex") == 0) {
            oct_to_hex(tc.input1, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: oct_to_hex(\"%s\") -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else if (strcmp(tc.function_name, "hex_to_bin") == 0) {
            hex_to_bin(tc.input1, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: hex_to_bin(\"%s\") -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else if (strcmp(tc.function_name, "to_sign_magnitude") == 0) {
            int32_t n = (int32_t)strtol(tc.input1, NULL, 10);
            to_sign_magnitude(n, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: to_sign_magnitude(%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else if (strcmp(tc.function_name, "to_ones_complement") == 0) {
            int32_t n = (int32_t)strtol(tc.input1, NULL, 10);
            to_ones_complement(n, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: to_ones_complement(%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else if (strcmp(tc.function_name, "to_twos_complement") == 0) {
            int32_t n = (int32_t)strtol(tc.input1, NULL, 10);
            to_twos_complement(n, actual);
            is_pass = (strcmp(actual, tc.expected) == 0);
            printf("Test %d: to_twos_complement(%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                   test_id, tc.input1, tc.expected, actual, is_pass ? "PASS" : "FAIL");
        } else {
            printf("Test %d: Unknown function \"%s\" (skipped)\n", test_id, tc.function_name);
            --total; // don't count
            continue;
        }

        if (is_pass) ++passed;
    }

    fclose(f);
    printf("Summary: %d/%d tests passed\n", passed, total);
    return (passed == total) ? 0 : 2;
}
