# CS 3503 – Assignment 2: Data Representation & Mapping

**Author:** Hazim Mahmood

Implements direct mapping conversions and 32‑bit signed representations in C.

## What's Included
- `convert.c` / `convert.h` – All required functions
- `main.c` – Test runner that parses a text test file (reusing the A1 pattern)
- (Use your provided) `a2_test.txt` – Test cases file

## Build
```bash
gcc -Wall -Wextra -O2 -o run main.c convert.c
```

## Run
```bash
./run /mnt/data/a2_test.txt
# or copy a2_test.txt next to the binary and run: ./run a2_test.txt
```

## Functions Implemented
- `oct_to_bin(const char* oct, char* out)` – 3-bit per octal digit (keeps leading zeros per digit).
- `oct_to_hex(const char* oct, char* out)` – via binary, left‑pad to nibble, uppercase hex, trim leading zeros except "0".
- `hex_to_bin(const char* hex, char* out)` – 4-bit per hex digit, case-insensitive.
- `to_sign_magnitude(int32_t n, char* out)` – MSB is sign, remaining bits are magnitude (32‑bit string).
- `to_ones_complement(int32_t n, char* out)` – Negative => flip all bits of magnitude’s 32‑bit form.
- `to_twos_complement(int32_t n, char* out)` – Direct cast to `uint32_t` then print bits.

## Notes
- Outputs match the provided A2 tests (e.g., `-5` two's: `...11111011`). 
- The sign‑magnitude form of `INT_MIN` isn't representable in 31 magnitude bits; the assignment tests don’t require that edge.
