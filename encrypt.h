#ifndef __ENCRYPT__LIB__
#define __ENCRYPT__LIB__

#include "galois.h"
#include "bmp_parser.h"


Shades encrypt(uint8_t * secret, size_t size, Shades initial_shades, int k);


#endif