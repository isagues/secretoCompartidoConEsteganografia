#ifndef __SHADES__LIB__
#define __SHADES__LIB__

#include <stdint.h>
#include "bmp_parser.h"

#define SHADES_NUMBER 8

typedef struct ShadeBlock {
    uint8_t x;
    uint8_t w;
    uint8_t v;
    uint8_t u;
} ShadeBlock;


ShadeBlock get_shadeblock_from_index(BMPImage image, size_t index);

void set_shadeblock_from_index(BMPImage image, size_t index, ShadeBlock newShadeBlock);

ShadeBlock distribute_t_value(ShadeBlock shadeBlock, uint8_t auxT);

ShadeBlock guarantee_different_x_values(ShadeBlock shadeBlock, uint8_t * xValues, size_t limit);

#endif