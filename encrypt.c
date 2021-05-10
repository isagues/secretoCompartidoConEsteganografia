#include "encrypt.h"
#define SHADES_NUMBER 8
#define GENERATOR_POL 0x163

//source: https://stackoverflow.com/questions/29379006/calculate-parity-bit-from-string-in-c/29381551#29381551
#define PARITY_BIT(t) (0x6996u >> ((t ^ (t >> 4)) & 0xf)) & 1


typedef struct ShadeBlock
{
    uint8_t x;
    uint8_t w;
    uint8_t v;
    uint8_t u;
} ShadeBlock;


static void get_shadeblock_indexes(BMPImage image, size_t index, size_t * x_row, size_t * x_col){
    //Divido la imagen a la mitad para separarla en cantidad de bloques

    //En row es division porque hasta que no se pase de cantidad de bloques esta en la misma fila
    //Se resta al height porque los pixeles estan ordenados al reves
    x_row = image.height - (index / (image.width / 2)) * 2;

    //En col es modulo porque tengo que ver en que bloque caigo
    x_col = (index % (image.width / 2)) * 2;
}

static ShadeBlock get_shadeblock_from_index(BMPImage image, size_t index){
    
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

static void set_shadeblock_from_index(BMPImage image, size_t index, ShadeBlock newShadeBlock){
    size_t x_row, x_col;

    get_shadeblock_indexes(image, index, &x_row, &x_col);

    image.data[x_row][x_col] = newShadeBlock.x;
    image.data[x_row][x_col + 1] = newShadeBlock.w;
    image.data[x_row - 1][x_col] = newShadeBlock.v;
    image.data[x_row - 1][x_col + 1] = newShadeBlock.u;

}

static ShadeBlock distribute_t_value(ShadeBlock shadeBlock, uint8_t auxT, uint8_t parityBit){
    
    shadeBlock.w &= 0xF8;
    shadeBlock.w |= (auxT >> 5) & 0x07;
    
    shadeBlock.v &= 0xF8;
    shadeBlock.v |= (auxT >> 2) & 0x07;
    
    shadeBlock.u &= 0xF8;
    shadeBlock.u |= auxT & 0x03;
    shadeBlock.u |= parityBit << 2;
    
    return shadeBlock;
}

Shades encrypt(uint8_t * secret, size_t size, Shades initial_shades, int k) {

    if((size % k) != 0) {
        printf("Number k of shades is not a multiple of %d", SHADES_NUMBER);
        return initial_shades;
    }
    uint8_t * secretBlock = malloc(k*sizeof(uint8_t));
    ShadeBlock shadeBlock;
    uint8_t auxT;
    uint8_t parityBit;

    for (size_t j = 0; j < size; j+=k)
    {
        memcpy(secretBlock, secret + j, k); //1 conseguir k pixeles del offset
        

        // Indices de esta forma para poder seguir con la convencion del paper
        for (size_t i = 0; i < initial_shades.size; i++)
        {
            shadeBlock = get_shadeblock_from_index(initial_shades.images[i], j); //2 conseguir las SHADES_NUMBER matrices de 2*2
            auxT = encrypt_function(shadeBlock.x, secretBlock); // s1 + s2*X + ... +sk*X^k-1 mod g(x)
            
            //calcular bit de paridad de auxT
            parityBit = PARITY_BIT(auxT);
            //calcular nuevos valores del shadeBlock
            shadeBlock = distribute_t_value(shadeBlock, auxT, parityBit);

            set_shadeblock_from_index(initial_shades.images[i], j, shadeBlock);

        }
        
        
    }
    


}