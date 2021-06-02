#ifndef __SHADES__LIB__
#define __SHADES__LIB__

#include <stdint.h>
#include "bmp_parser/bmp_parser.h"

#define SHADE_BLOCK_SIZE 4

typedef struct ShadeBlock {
    uint8_t x;
    uint8_t w;
    uint8_t v;
    uint8_t u;
} ShadeBlock;


ShadeBlock shadeblock_get(BMPImage *image, size_t index);

void shadeblock_set(BMPImage *image, size_t index, ShadeBlock *shadeBlock);

void shadeblock_distribute_t_value(ShadeBlock *shadeBlock, uint8_t tValue);

uint8_t shadeblock_recover_t_value(ShadeBlock *shadeBlock);

void shadeblock_guarantee_different_x_values(ShadeBlock *shadeBlock, uint8_t * xValues, size_t limit);

#endif