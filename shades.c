#include "shades.h"

static void get_shadeblock_indexes(BMPImage image, size_t index, size_t * x_row, size_t * x_col){
    //Divido la imagen a la mitad para separarla en cantidad de bloques

    //En row es division porque hasta que no se pase de cantidad de bloques esta en la misma fila
    //Se resta al height porque los pixeles estan ordenados al reves
    *x_row = (image.height - 1) - (index / (image.width / 2)) * 2;

    //En col es modulo porque tengo que ver en que bloque caigo
    *x_col = (index % (image.width / 2)) * 2;
}

ShadeBlock get_shadeblock_from_index(BMPImage image, size_t index){
    
    size_t x_row, x_col;

    get_shadeblock_indexes(image, index, &x_row, &x_col);
   
    ShadeBlock shadeBlock;

    shadeBlock.x = image.data[x_row][x_col];
    shadeBlock.w = image.data[x_row][x_col + 1];

    //Se resta porque las imagenes tienen los pixeles al reves, las columnas si estan en orden
    shadeBlock.v = image.data[x_row - 1][x_col];
    shadeBlock.u = image.data[x_row - 1][x_col + 1];

    return shadeBlock;
}

void set_shadeblock_from_index(BMPImage image, size_t index, ShadeBlock newShadeBlock){
    size_t x_row, x_col;

    get_shadeblock_indexes(image, index, &x_row, &x_col);

    image.data[x_row][x_col] = newShadeBlock.x;
    image.data[x_row][x_col + 1] = newShadeBlock.w;
    image.data[x_row - 1][x_col] = newShadeBlock.v;
    image.data[x_row - 1][x_col + 1] = newShadeBlock.u;

}

ShadeBlock distribute_t_value(ShadeBlock shadeBlock, uint8_t auxT, uint8_t parityBit){
    
    shadeBlock.w &= 0xF8;                   // 0xF8 = 1111 1000
    shadeBlock.w |= (auxT >> 5) & 0x07;     // 0x07 = 0000 0111
    
    shadeBlock.v &= 0xF8;
    shadeBlock.v |= (auxT >> 2) & 0x07;
    
    shadeBlock.u &= 0xF8;
    shadeBlock.u |= auxT & 0x03;            // 0x03 = 0000 0011
    shadeBlock.u |= parityBit << 2;
    
    return shadeBlock;
}

ShadeBlock guarantee_different_x_values(ShadeBlock shadeBlock, uint8_t * xValues, size_t limit){
    
    int valueIsPresent = 0;
    
    for (size_t i = 0; i < limit; i++) {
        
        if (shadeBlock.x == xValues[i])
            valueIsPresent = 1;

        // TODO(nacho, faus): Se podria mejorar para que cuando llegue a 255 no salte a 0 y cambie el color abrutamente.
        if(valueIsPresent){
            shadeBlock.x = shadeBlock.x + 1 % 255;
            i = 0;
            valueIsPresent = 0;
        }
    }
    return shadeBlock;
}