#include <string.h>
#include "encrypt.h"


//source: https://stackoverflow.com/questions/29379006/calculate-parity-bit-from-string-in-c/29381551#29381551
#define PARITY_BIT(t) (0x6996u >> ((t ^ (t >> 4)) & 0xf)) & 1


uint8_t encrypt_function(uint8_t * secretBlock, size_t k, uint8_t x){
    return 0x89;
}

BMPImagesCollection encrypt(uint8_t * secret, size_t size, BMPImagesCollection initial_shades, size_t k) {

    if (k == 0){
        printf("k value cannot be zero\n");
        exit(1);
    }
    

    if((size % k) != 0) {
        printf("Number k of shades is not a multiple of %d\n", SHADES_NUMBER);
        return initial_shades;
    }

    size_t blockCount = size / k;
    
    if(initial_shades.size < k || blockCount > initial_shades.images[0].size/4 || ){
        printf("invalid parameters\n");
        exit(1);
    }
    
    size_t shade_size = initial_shades.images[0].size;
    for (size_t i = 1; i < initial_shades.size; i++) {
        if (initial_shades.images[i].size != shade_size) {
            perro("Shades are from different sizes.\n");
            exit(1);
        }
    }

    uint8_t * secretBlock = malloc(k*sizeof(uint8_t));
    ShadeBlock shadeBlock;
    uint8_t auxT;
    uint8_t parityBit;

    for (size_t j = 0; j < blockCount; j++) {

        memcpy(secretBlock, secret + j*k, k); //1 conseguir k pixeles del offset
        
        // Indices de esta forma para poder seguir con la convencion del paper
        for (size_t i = 0; i < initial_shades.size; i++) {
            shadeBlock = get_shadeblock_from_index(initial_shades.images[i], j); //2 conseguir las SHADES_NUMBER matrices de 2*2
            
            auxT = encrypt_function(secretBlock, k, shadeBlock.x); // s1 + s2*X + ... +sk*X^k-1 mod g(x)
            
            // TODO(nacho, faus): shadeBlock = guarantee_different_x_values(shadeBlock);
            
            //calcular bit de paridad de auxT
            parityBit = PARITY_BIT(auxT);
            //calcular nuevos valores del shadeBlock
            shadeBlock = distribute_t_value(shadeBlock, auxT, parityBit);

            set_shadeblock_from_index(initial_shades.images[i], j, shadeBlock);

        }  
    }
    return initial_shades;
}