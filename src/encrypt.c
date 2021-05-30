#include "encrypt.h"

#include <string.h>

uint8_t encrypt_function(uint8_t * secretBlock, size_t k, uint8_t x) {
    return galois_poly_eval(secretBlock, k, x);    // s1 + s2*X + ... +sk*X^k-1 mod g(x)
}

bool encrypt(uint8_t *secret, size_t size, BMPImagesCollection *shades, uint8_t k) {

    //TODO(tobi): Manejar distintos generadores - Llamar a galois_set_generator(generator);
    // TODO(tobi): Agregar padding para manejar cualquier valor de k
    // TODO(tobi): Ponerle tope a k, para que no haya stack overflow
    if(k == 0  || (size % k) != 0) {
        printf("invalid value for k = %c\n", k);
        return false;
    }
    
    size_t blockCount = size / k;
    
    if(shades->size < k || blockCount > shades->images[0].size / 4){
        printf("invalid parameters\n");
        return false;
    }
    
    size_t shade_size = shades->images[0].size;
    for(size_t i = 1; i < shades->size; i++) {
        if (shades->images[i].size != shade_size) {
            perror("Shades are from different sizes.\n");
            return false;
        }
    }

    //secret block y xValues se inicializan directamente a pesar de ser variable porque es un uint_8 
    //y no van a ser demasiados bytes
    uint8_t secretBlock[k];
    ShadeBlock shadeBlock;
    uint8_t tValue;
    uint8_t xValues[shades->size];

    for(size_t j = 0; j < blockCount; j++) {
        
        memset(xValues, 0, shades->size);

        memcpy(secretBlock, secret + j*k, k); // 1. conseguir k pixeles del offset
        
        // Indices de esta forma para poder seguir con la convencion del paper
        for(size_t i = 0; i < shades->size; i++) {
            shadeBlock = shadeblock_get(&shades->images[i], j); // 2. conseguir las SHADES_NUMBER matrices de 2*2
            
            shadeblock_guarantee_different_x_values(&shadeBlock, xValues, i);

            xValues[i] = shadeBlock.x;

            tValue = encrypt_function(secretBlock, k, shadeBlock.x); // s1 + s2*X + ... +sk*X^k-1 mod g(x)
            
            // Distribuir el valor calculado en el shade block
            shadeblock_distribute_t_value(&shadeBlock, tValue);

            shadeblock_set(&shades->images[i], j, &shadeBlock);
        }  
    }

    return true;
}

bool shades_persist(char * dirPath, BMPImagesCollection *shades, BMPHeader *header){
    bool ret = true;
    char auxPath[PATH_MAX];
    
    // TODO(tobi): mkdir if dir doesn't exists
    // Capaz borrar los archivos creados si hubo un error? Probablemente no, paja
    for(size_t i = 0; ret && i < shades->size; i++) {
        sprintf(auxPath, "%s/shade_%ld.bmp", dirPath, i);
        ret &= bmp_persist_image(auxPath, header, &shades->images[i]);
    }

    return ret;
}

uint8_t * decrypt(size_t size, BMPImagesCollection *shades, uint8_t k) {

    if(k == 0 || (size % k) != 0) {
        printf("invalid value for k = %c\n", k);
        return NULL;
    }
    
    size_t blockCount = size / k;
    
    if(shades->size < k || blockCount > shades->images[0].size / 4){
        printf("invalid parameters\n");
        return NULL;
    }
    
    size_t shade_size = shades->images[0].size;

    for (size_t i = 1; i < shades->size; i++) {
        if(shades->images[i].size != shade_size) {
            perror("Shades are from different sizes.\n");
            return NULL;
        }
    }

    uint8_t *secret = malloc(size * sizeof(*secret));
    if(secret == NULL) {
        return NULL;
    }

    uint8_t secretBlock[k];
    ShadeBlock shadeBlock;
    uint8_t xValues[k];
    uint8_t yValues[k];

    for(size_t j = 0; j < blockCount; j++) {

        // Indices de esta forma para poder seguir con la convencion del paper
        for(size_t i = 0; i < k; i++) {
            shadeBlock = shadeblock_get(&shades->images[i], j);
            
            xValues[i] = shadeBlock.x;
            yValues[i] = shadeblock_recover_t_value(&shadeBlock);
        }  

        galois_lagrange_interpolation(xValues, yValues, secretBlock, k);

        memcpy(secret + j*k, secretBlock, k);
    }

    return secret;
}