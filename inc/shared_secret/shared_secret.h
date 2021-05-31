#ifndef __ENCRYPT__LIB__
#define __ENCRYPT__LIB__

#include "galois/galois.h"
#include "bmp_parser/bmp_parser.h"


bool ss_distribute(uint8_t * secret, size_t size, BMPImagesCollection *shades, uint8_t k, bool padding);

bool shades_persist(char * dirPath, BMPImagesCollection *shades, BMPHeader *header);

uint8_t * ss_recover(size_t size, BMPImagesCollection *shades, uint8_t k, bool padding);

#endif