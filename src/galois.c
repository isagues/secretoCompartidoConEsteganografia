#include <galois.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

galois2_8_gen_t galois_generators[GAL_GENERATOR_COUNT] = {
    0x003, 0x007, 0x00B, 0x00D, 0x013, 0x019, 0x025, 0x029, 0x02F,
    0x037, 0x03B, 0x03D, 0x043, 0x05B, 0x061, 0x067, 0x06D, 0x073,
    0x083, 0x089, 0x08F, 0x091, 0x09D, 0x0A7, 0x0AB, 0x0B9, 0x0BF,
    0x0C1, 0x0CB, 0x0D3, 0x0D5, 0x0E5, 0x0EF, 0x0F1, 0x0F7, 0x0FD,
    0x11B, 0x11D, 0x12B, 0x12D, 0x14C, 0x15F, 0x163, 0x165, 0x169,
    0x171, 0x187, 0x18D, 0x1A9, 0x1C3, 0x1CF, 0x1E7, 0x1F5,
};

static galois2_8_gen_t gal_gen = GAL_DEFAULT_GENERATOR;

// Pre-calculated gal_inv. calculate_multiplicative_inverses(gal_inv) using GAL_DEFAULT_GENERATOR
static galois2_8_t gal_inv[16][16] = {
    {0x00, 0x01, 0xb1, 0xde, 0xe9, 0x4a, 0x6f, 0x8c, 0xc5, 0xa5, 0x25, 0xc1, 0x86, 0x54, 0x46, 0xe7, },
    {0xd3, 0x5d, 0xe3, 0x85, 0xa3, 0x34, 0xd1, 0xed, 0x43, 0xa6, 0x2a, 0x63, 0x23, 0x9e, 0xc2, 0x77, },
    {0xd8, 0x2d, 0x9f, 0x1c, 0xc0, 0x0a, 0xf3, 0xab, 0xe0, 0xb5, 0x1a, 0x62, 0xd9, 0x21, 0xc7, 0xbd, },
    {0x90, 0x56, 0x53, 0xea, 0x15, 0xa2, 0x80, 0x79, 0xa0, 0x5b, 0x4f, 0xe5, 0x61, 0x49, 0x8a, 0xcd, },
    {0x6c, 0x9a, 0xa7, 0x18, 0xfe, 0x7c, 0x0e, 0xe6, 0x60, 0x3d, 0x05, 0xe8, 0xc8, 0x82, 0xe4, 0x3a, },
    {0x70, 0x6a, 0xeb, 0x32, 0x0d, 0x87, 0x31, 0x91, 0xdd, 0xf6, 0xa1, 0x39, 0xd2, 0x11, 0xef, 0xbf, },
    {0x48, 0x3c, 0x2b, 0x1b, 0x98, 0xd5, 0x75, 0x97, 0xbb, 0xb8, 0x51, 0x71, 0x40, 0x9b, 0x8d, 0x06, },
    {0x50, 0x6b, 0x9c, 0x8e, 0x96, 0x66, 0xc3, 0x1f, 0x81, 0x37, 0x95, 0xb2, 0x45, 0x7f, 0xd7, 0x88, },
    {0x36, 0x78, 0x4d, 0xc9, 0xe2, 0x13, 0x0c, 0x55, 0x7f, 0xd6, 0x3e, 0xcc, 0x07, 0x6e, 0x73, 0x9d, },
    {0x30, 0x57, 0xaf, 0xf8, 0xb3, 0x7a, 0x74, 0x67, 0x64, 0xd4, 0x41, 0x6d, 0x72, 0x8f, 0x1d, 0x22, },
    {0x38, 0x5a, 0x35, 0x14, 0xc4, 0x09, 0x19, 0x42, 0xb7, 0xac, 0xf2, 0x27, 0xa9, 0xb6, 0xf9, 0x92, },
    {0xdf, 0x02, 0x7b, 0x94, 0xe1, 0x29, 0xad, 0xa8, 0x69, 0xba, 0xb9, 0x68, 0xc6, 0x2f, 0xee, 0x5f, },
    {0x24, 0x0b, 0x1e, 0x76, 0xa4, 0x08, 0xbc, 0x2e, 0x4c, 0x83, 0xdb, 0xfd, 0x8b, 0x3f, 0xfa, 0xf5, },
    {0xec, 0x16, 0x5c, 0x10, 0x99, 0x65, 0x89, 0x7e, 0x20, 0x2c, 0xfc, 0xca, 0xf7, 0x58, 0x03, 0xb0, },
    {0x28, 0xb4, 0x84, 0x12, 0x4e, 0x3b, 0x47, 0x0f, 0x4b, 0x04, 0x33, 0x52, 0xd0, 0x17, 0xbe, 0x5e, },
    {0xf1, 0xf0, 0xaa, 0x26, 0xfb, 0xcf, 0x59, 0xdc, 0x93, 0xae, 0xce, 0xf4, 0xda, 0xcb, 0x44, 0x7d, },
};

static void calculate_multiplicative_inverses(galois2_8_t ginv[16][16]);

void galois_set_generator(galois2_8_gen_t generator) {
    // TODO(tobi): Chequear que sea un gen valido
    if(generator != gal_gen) {
        gal_gen = generator;
        galois_multiplicative_inverses(gal_inv);
    }
}

// https://en.wikipedia.org/wiki/Finite_field_arithmetic#Program_examples
/* Add two numbers in the GF(2^8) finite field */
galois2_8_t gadd(galois2_8_t a, galois2_8_t b) {
	return a ^ b;
}

galois2_8_t gsub(galois2_8_t a, galois2_8_t b) {
	return a ^ b;
}

/* Multiply two numbers in the GF(2^8) finite field defined 
 * by the polynomial gal_gen
 * using the Russian Peasant Multiplication algorithm
 * (the other way being to do carry-less multiplication followed by a modular reduction)
 */
galois2_8_t gmul(galois2_8_t a, galois2_8_t b) {
	galois2_8_t p = 0; /* the product of the multiplication */
	while (a && b) {
            if (b & 1) /* if b is odd, then add the corresponding a to p (final product = sum of all a's corresponding to odd b's) */
                p ^= a; /* since we're in GF(2^m), addition is an XOR */

            if (a & 0x80) /* GF modulo: if a >= 128, then it will overflow when shifted left, so reduce */
                a = (a << 1) ^ gal_gen; /* XOR with the generator */
            else
                a <<= 1; /* equivalent to a*2 */
            b >>= 1; /* equivalent to b // 2 */
	}
	return p;
}

galois2_8_t ginv(galois2_8_t a) {
	return gal_inv[(a & 0xF0) >> 4][a & 0x0F];
}

galois2_8_t gdiv(galois2_8_t a, galois2_8_t b) {
    if(b == GAL_SUM_ID) {
        printf("Division by 0");
        exit(1);
    }
	return gmul(a, ginv(b));
}

galois2_8_t gpow(galois2_8_t base, galois2_8_t exp) {
    // TODO(tobi): Mejorar
    galois2_8_t result = GAL_MUL_ID;
    for (uint8_t i = 0; i < exp; i++) {
        result = gmul(result, base);
    }
    return result;
}

galois2_8_t galois_poly_eval(galois2_8_t coeficients[], uint8_t n, galois2_8_t x_value) {
    // TODO(tobi): Mejorar
    galois2_8_t result = coeficients[0];
    
    for (uint8_t i = 1; i < n; i++) {
        result = gadd(result, gmul(coeficients[i], gpow(x_value, i)));
    }
    return result;
}

/**
 * Generates a table with all multiplicatives inverses in the field
 * 
 * For example, for the galois member 0xA3, it's inverse is ginv[0x0A][0x03]
 * Then, gmul(0xA3, ginv[0x0A][0x03]) = GAL_MUL_ID = 0x01
 * 
 * O(16 * 16 * 256) - Naive
 */ 
static void calculate_multiplicative_inverses(galois2_8_t ginv[16][16]) {
    // EL inverso de la suma no tiene inverso multiplicativo
    ginv[0][0] = GAL_SUM_ID;

    for(uint8_t x = 0; x <= 0x0F; x++) {
        for(uint8_t y = 1; y <= 0x0F; y++) {
            // Pruebo posibles numeros de galois hasta que le pego al inverso
            for(galois2_8_t inv = 0; inv < GAL_MAX_VAL; inv++) {
                if(gmul((x << 4) | y, inv) == GAL_MUL_ID) {
                    ginv[x][y] = inv;
                }
            }
        }
    }
}

void galois_multiplicative_inverses(galois2_8_t ginv[16][16]) {
    memcpy(ginv, gal_inv, sizeof(gal_inv));
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
    galois2_8_t ret = GAL_MUL_ID;
    for(uint8_t q = 0; q < n - r; q++) {
        if(q != i) {
            // ret *= x[q] / (x[i] - x[q]);
            ret = gmul(ret, gdiv(x[q], gsub(x[i], x[q])));
        }
    }

    return ret;
}

void galois_lagrange_interpolation(galois2_8_t x[], galois2_8_t y[], galois2_8_t p[], uint8_t n) {

    // r = coefficient_idx
    for(uint8_t r = 0; r < n; r++) {
        galois2_8_t coefficient = 0;
        //MANnuver
        galois2_8_t y_prime[n];


        for(uint8_t i = 0; i < n - r; i++) {
            // No seria cuando r es > 0?
            if(r > 0) {
                // y_prime = (y[i] - p[0]) / x[i];
                y_prime[i] = gdiv(gsub(y_prime[i], p[r - 1]), x[i]);
            }
            else {
                y_prime[i] = y[i];
            }

            // coefficient += y_prime * lagrange_interpolation_coefficient_intermediate_product(x, n, r, p);
            coefficient = gadd(coefficient, gmul(y_prime[i], lagrange_interpolation_coefficient_intermediate_product(x, n, r, i)));
        }

        // No es necesario pues en G(2^8) un numero es su propio inverso en la suma
        /* if(n - r % 2 == 0) {
        *     coefficient = -coefficient;
        *  }
        */

       p[r] = coefficient;
    }
}
