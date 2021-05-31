#include "shared_secret/shades.h"
#include "log/log.h"

//source: https://stackoverflow.com/questions/29379006/calculate-parity-bit-from-string-in-c/29381551#29381551
#define PARITY_BIT(t) ((0x6996u >> ((t ^ (t >> 4)) & 0xf)) & 1)

static void shadeblock_coordinates(BMPImage *image, size_t index, size_t * x_row, size_t * x_col){
    //Divido la imagen a la mitad para separarla en cantidad de bloques

    //En row es division porque hasta que no se pase de cantidad de bloques esta en la misma fila
    //Se resta al height porque los pixeles estan ordenados al reves
    *x_row = (image->height - 1) - (index / (image->width / 2)) * 2;

    //En col es modulo porque tengo que ver en que bloque caigo
    *x_col = (index % (image->width / 2)) * 2;
}

ShadeBlock shadeblock_get(BMPImage *image, size_t index){
    
    size_t x_row, x_col;
    shadeblock_coordinates(image, index, &x_row, &x_col);
   
    ShadeBlock shadeBlock;

    shadeBlock.x = image->data[x_row][x_col];
    shadeBlock.w = image->data[x_row][x_col + 1];

    //Se resta porque las imagenes tienen los pixeles al reves, las columnas si estan en orden
    shadeBlock.v = image->data[x_row - 1][x_col];
    shadeBlock.u = image->data[x_row - 1][x_col + 1];

    return shadeBlock;
}

void shadeblock_set(BMPImage *image, size_t index, ShadeBlock *shadeBlock){
    size_t x_row, x_col;
    shadeblock_coordinates(image, index, &x_row, &x_col);

    image->data[x_row]    [x_col]       = shadeBlock->x;
    image->data[x_row]    [x_col + 1]   = shadeBlock->w;
    image->data[x_row - 1][x_col]       = shadeBlock->v;
    image->data[x_row - 1][x_col + 1]   = shadeBlock->u;
}

void shadeblock_distribute_t_value(ShadeBlock *shadeBlock, uint8_t tValue){

    //calcular bit de paridad de auxT
    uint8_t parityBit = PARITY_BIT(tValue);
    
    shadeBlock->w &= 0xF8;                   // 0xF8 = 1111 1000
    shadeBlock->w |= (tValue >> 5) & 0x07;   // 0x07 = 0000 0111
    
    shadeBlock->v &= 0xF8;
    shadeBlock->v |= (tValue >> 2) & 0x07;
    
    shadeBlock->u &= 0xF8;
    shadeBlock->u |= tValue & 0x03;          // 0x03 = 0000 0011
    shadeBlock->u |= parityBit << 2;
}

uint8_t shadeblock_recover_t_value(ShadeBlock *shadeBlock) {
    uint8_t y = 0;
    
    y |= (shadeBlock->w & 0x07) << 5;        // y = w6w7w8xxxxx
    y |= (shadeBlock->v & 0x07) << 2;        // y = w6w7w8v6v7v8xx
    y |= (shadeBlock->u & 0x03);             // y = w6w7w8v6v7v8u7u8
    
    if(PARITY_BIT(y) != (shadeBlock->u & 0x04) >> 2) {
        LOG_ERROR("Parity bit doesn't match");
        return 0; // Si no coincide, lo ignoramos
    }
    
    return y;
}

void shadeblock_guarantee_different_x_values(ShadeBlock *shadeBlock, uint8_t *xValues, size_t limit) {
    bool valueIsPresent = false;
    int direction = (shadeBlock->x > UINT8_MAX / 2) ? -1 : 1;
    
    for(size_t i = 0; i < limit; i++) {
        
        if(shadeBlock->x == xValues[i]) {
            valueIsPresent = true;
        }

        if(valueIsPresent) {
            i = 0;
            valueIsPresent = false;

            do {
                shadeBlock->x += direction;
            } while(shadeBlock->x == xValues[i]);
        }
    }
}
