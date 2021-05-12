// TODO(tobi): Agregar header - libreria operaciones galois G(2^8)
#ifndef __TEST_UTILS__
#define __TEST_UTILS__

#include <stdint.h>
#include <stddef.h>

#define N(arr) (sizeof(arr)/sizeof(arr[0]))

void assert_equal_uint8_array(uint8_t expected[], uint8_t actual[], size_t n);

void print_uint8_array(uint8_t arr[], size_t n);

#endif