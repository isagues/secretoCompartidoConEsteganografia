// TODO(tobi): Agregar header - libreria operaciones galois G(2^8)
#ifndef __GALOIS__LIB__
#define __GALOIS__LIB__

#include <stdint.h>

#define GAL_GENERATOR_COUNT 53

#define GAL_DEFAULT_GENERATOR 0x163

#define GAL_MAX_VAL 0xFF

#define GAL_MUL_ID 0x01
#define GAL_SUM_ID 0x00

typedef uint8_t galois2_8_t;

typedef uint16_t galois2_8_gen_t;

extern galois2_8_gen_t galois_generators[GAL_GENERATOR_COUNT];

//sources: https://en.wikipedia.org/wiki/Finite_field_arithmetic

void galois_set_generator(galois2_8_gen_t generator);

galois2_8_t gadd(galois2_8_t a, galois2_8_t b);

galois2_8_t gmul(galois2_8_t a, galois2_8_t b);

galois2_8_t ginv(galois2_8_t a);

galois2_8_t gdiv(galois2_8_t a, galois2_8_t b);

galois2_8_t gpow(galois2_8_t base, galois2_8_t exp);

void galois_multiplicative_inverses(galois2_8_t ginv[16][16]);

galois2_8_t galois_poly_eval(galois2_8_t coeficients[], uint8_t n, galois2_8_t x_value);


/**
 * Custom Implementation of Lagrange Interpolation in Galois Field G(2^8)\n
 * 
 * Calculates the Lagrange Polynomial P(x) such that P(x[i]) = y[i] \n
 * 
 * (https://en.wikipedia.org/wiki/Lagrange_polynomial)
 * 
 * @param x     Points to interpolate in G(2^8). They must be unique.
 * @param y     Values to interpolate in G(2^8)
 * @param p     P(x) coefficients are saved in ascending order here
 * @param n     Number of points and values to interpolate, and P(x)'s degree. 
 *              All arrays must be of at least this size.
 */
void galois_lagrange_interpolation(galois2_8_t x[], galois2_8_t y[], galois2_8_t p[], uint8_t n);

#endif