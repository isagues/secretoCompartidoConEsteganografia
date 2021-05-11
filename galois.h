#ifndef __GALOIS__LIB__
#define __GALOIS__LIB__

#include <stdint.h>

#define GENERATOR_POL 0x163

//sources: https://en.wikipedia.org/wiki/Finite_field_arithmetic

uint8_t gadd(uint8_t a, uint8_t b);

uint8_t gmul(uint8_t a, uint8_t b);

uint8_t gpow(uint8_t base, uint8_t exp);

uint8_t evaluate_pol(uint8_t * coeficients, uint8_t size, uint8_t x_value);

#endif