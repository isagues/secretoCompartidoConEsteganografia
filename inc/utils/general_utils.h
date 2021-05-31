#ifndef __UTILS__LIB__
#define __UTILS__LIB__

#include <stdint.h>
#include <stddef.h>

#define N(arr) (sizeof(arr)/sizeof(arr[0]))

void print_uint8_array(uint8_t arr[], size_t n);

#endif