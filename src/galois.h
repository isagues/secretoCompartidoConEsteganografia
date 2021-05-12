// TODO(tobi): Agregar header - libreria operaciones galois G(2^8)
#ifndef __GALOIS__LIB__
#define __GALOIS__LIB__

#include <stdint.h>

#define GENERATOR_POL 0x163

#define MUL_IDENTITY 1
#define SUM_IDENTITY 0

typedef uint8_t galois2_8_t;

//sources: https://en.wikipedia.org/wiki/Finite_field_arithmetic

galois2_8_t gadd(galois2_8_t a, galois2_8_t b);

galois2_8_t gmul(galois2_8_t a, galois2_8_t b);

galois2_8_t gpow(galois2_8_t base, galois2_8_t exp);

galois2_8_t evaluate_pol(galois2_8_t coeficients[], uint8_t n, galois2_8_t x_value);

/**
 * Custom Implementation of Lagrange Interpolation in Galois Field G(2^8)\n
 * 
 * Calculates the Lagrange Polynomial P(x) such that P(x[i]) = y[i] \n
 * 
 * (https://en.wikipedia.org/wiki/Lagrange_polynomial)
 * 
 * @param x     Points to interpolate in G(2^8)
 * @param y     Values to interpolate in G(2^8)
 * @param p     P(x) coefficients are saved in ascending order here
 * @param n     Number of points and values to interpolate, and P(x)'s degree. 
 *              All arrays must be of at least this size.
 */
void lagrange_interpolation(galois2_8_t x[], galois2_8_t y[], galois2_8_t p[], uint8_t n);

#endif