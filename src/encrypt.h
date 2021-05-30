#ifndef __ENCRYPT__LIB__
#define __ENCRYPT__LIB__

#include "galois.h"
#include "bmp_parser.h"
#include "shades.h"


bool encrypt(uint8_t * secret, size_t size, BMPImagesCollection *shades, uint8_t k);

void shades_persist(char * dirPath, BMPImagesCollection *shades, BMPHeader *header);

uint8_t * decrypt(size_t size, BMPImagesCollection *shades, uint8_t k);

#endif