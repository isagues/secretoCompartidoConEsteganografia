#include "galois.h"

// https://en.wikipedia.org/wiki/Finite_field_arithmetic#Program_examples
/* Add two numbers in the GF(2^8) finite field */
galois2_8_t gadd(galois2_8_t a, galois2_8_t b) {
	return a ^ b;
}

galois2_8_t gsub(galois2_8_t a, galois2_8_t b) {
	return a ^ b;
}

/* Multiply two numbers in the GF(2^8) finite field defined 
 * by the polynomial x^8 + x^4 + x^3 + x + 1 = 0
 * using the Russian Peasant Multiplication algorithm
 * (the other way being to do carry-less multiplication followed by a modular reduction)
 */
galois2_8_t gmul(galois2_8_t a, galois2_8_t b) {
	galois2_8_t p = 0; /* the product of the multiplication */
	while (a && b) {
            if (b & 1) /* if b is odd, then add the corresponding a to p (final product = sum of all a's corresponding to odd b's) */
                p ^= a; /* since we're in GF(2^m), addition is an XOR */

            if (a & 0x80) /* GF modulo: if a >= 128, then it will overflow when shifted left, so reduce */
                a = (a << 1) ^ GENERATOR_POL; /* XOR with the primitive polynomial x^8 + x^4 + x^3 + x + 1 (0b1_0001_1011) – you can change it but it must be irreducible */
            else
                a <<= 1; /* equivalent to a*2 */
            b >>= 1; /* equivalent to b // 2 */
	}
	return p;
}

galois2_8_t gdiv(galois2_8_t a, galois2_8_t b) {
    // TODO(tobi)
	return 1;
}

galois2_8_t gpow(galois2_8_t base, galois2_8_t exp) {
    int result = base;
    for (uint8_t i = 0; i < exp; i++) {
        result = gmul(result, base);
    }
    return result;
}

galois2_8_t evaluate_pol(galois2_8_t coeficients[], uint8_t n, galois2_8_t x_value) {
    uint8_t result = coeficients[0];
    
    for (uint8_t i = 1; i < n; i++) {
        result = gadd(result, gmul(coeficients[i], gpow(x_value, i)));
    }
    return result;
}

/**
 * Calcula la produtoria intermedia del termino i del coeficiente r.
 * 
 * @param x Puntos a interpolar
 * @param n Cantidad de puntos a interpolar
 * @param r Indice del coeficiente
 * @param i Indice del termino a calcular
 */
static galois2_8_t lagrange_interpolation_coefficient_intermediate_product(galois2_8_t x[], uint8_t n, uint8_t r, uint8_t i) {
    galois2_8_t ret = MUL_IDENTITY;

    for(uint8_t q = 0; q < n - r; q++) {
        // ret *= x[q] / (x[i] - x[q]);
        ret = gmul(ret, gdiv(x[q], gsub(x[i], x[q])));
    }

    return ret;
}

void lagrange_interpolation(galois2_8_t x[], galois2_8_t y[], galois2_8_t p[], uint8_t n) {

    // r = coefficient_idx
    for(uint8_t r = 0; r < n; r++) {
        galois2_8_t coefficient = 0;
        galois2_8_t y_prime = y[0];

        for(uint8_t i = 0; i < n - r; i++) {
            if(i > 0) {
                // y_prime = (y[i] + p[0]) / x[i];
                y_prime = gdiv(gsub(y[i], p[0]), x[i]);
            }

            // coefficient += y_prime * lagrange_interpolation_coefficient_intermediate_product(x, n, r, p);
            coefficient = gadd(coefficient, gmul(y_prime, lagrange_interpolation_coefficient_intermediate_product(x, n, r, i)));
        }

        // No es necesario pues en G(2^8) un numero es su propio inverso
        /* if(n - r % 2 == 0) {
        *     coefficient = -coefficient;
        *  }
        */

       p[r] = coefficient;
    }
}
