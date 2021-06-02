#ifndef __UTILS__LIB__
#define __UTILS__LIB__

#include <stdint.h>
#include <stddef.h>

#define N(arr) (sizeof(arr)/sizeof(arr[0]))

//source: https://stackoverflow.com/questions/29379006/calculate-parity-bit-from-string-in-c/29381551#29381551
#define PARITY_BIT(t) ((0x6996u >> ((t ^ (t >> 4)) & 0xf)) & 1)

#define TO_EVEN(x) (((x) >> 1) << 1)

void print_uint8_array(uint8_t arr[], size_t n);

#endif