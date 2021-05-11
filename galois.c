#include "galois.h"

// https://en.wikipedia.org/wiki/Finite_field_arithmetic#Program_examples
/* Add two numbers in the GF(2^8) finite field */
uint8_t gadd(uint8_t a, uint8_t b) {
	return a ^ b;
}

/* Multiply two numbers in the GF(2^8) finite field defined 
 * by the polynomial x^8 + x^4 + x^3 + x + 1 = 0
 * using the Russian Peasant Multiplication algorithm
 * (the other way being to do carry-less multiplication followed by a modular reduction)
 */
uint8_t gmul(uint8_t a, uint8_t b) {
	uint8_t p = 0; /* the product of the multiplication */
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

uint8_t gpow(uint8_t base, uint8_t exp) {
    int result = base;
    for (uint8_t i = 0; i < exp; i++)   
    {
        result = gmul(result, base);
    }
    return result;
}

uint8_t evaluate_pol(uint8_t * coeficients, uint8_t size, uint8_t x_value) {
    uint8_t result = coeficients[0];
    
    for (uint8_t i = 1; i < size; i++)
    {
        result = gadd(result, gmul(coeficients[i], gpow(x_value, i)));
    }
    return result;
}