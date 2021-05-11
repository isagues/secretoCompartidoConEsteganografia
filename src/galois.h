// TODO(tobi): Agregar header - libreria operaciones galois G(2^8)
#ifndef __GALOIS__LIB__
#define __GALOIS__LIB__

#include <stdint.h>

#define GENERATOR_POL 0x163

//sources: https://en.wikipedia.org/wiki/Finite_field_arithmetic

uint8_t gadd(uint8_t a, uint8_t b);

uint8_t gmul(uint8_t a, uint8_t b);

uint8_t gpow(uint8_t base, uint8_t exp);

uint8_t evaluate_pol(uint8_t * coeficients, uint8_t size, uint8_t x_value);

/**
 * Custom Implementation of Lagrange Interpolation \n
 * 
 * Calculates the Lagrange Polynomial P(x) such that P(x[i]) = y[i] \n
 * 
 * (https://en.wikipedia.org/wiki/Lagrange_polynomial)
 * 
 * @param x     Points to interpolate
 * @param y     Values to interpolate
 * @param p     P(x) coefficients are saved in ascending order here
 * @param n     Number of points and values to interpolate, and P(x)'s degree. 
 *              All arrays must be of at least this size.
 */
void lagrange_interpolation(uint8_t x[], uint8_t y[], uint8_t p[], uint8_t n);

#endif