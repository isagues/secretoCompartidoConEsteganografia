#include "encrypt.h"

#include <string.h>

static bool validate_input(size_t secretSize, BMPImagesCollection *shades, uint8_t k, bool padding);

uint8_t encrypt_function(uint8_t * secretBlock, size_t k, uint8_t x) {
    return galois_poly_eval(secretBlock, k, x);    // s1 + s2*X + ... +sk*X^k-1 mod g(x)
}

static bool validate_input(size_t secretSize, BMPImagesCollection *shades, uint8_t k, bool padding) {
    
    if(k < 1) {
        fprintf(stderr, "k must be at least 2\n");
        return false;
    }
    
    size_t remainder = secretSize % k;
    size_t blockCount = secretSize / k + !!remainder;

    if(remainder && !padding) {
        fprintf(stderr, "secret must be a divisible by k or padding must be enabled\n");
        return false;
    } 
    
    if(shades->size < k) {
        fprintf(stderr, "Not enough shades to process secret were provided. %d shades are needed.", k);
        return false;
    }

    size_t shadeSize = shades->images[0].size;

    if(blockCount > shadeSize / 4){
        fprintf(stderr, "Shades are not long enough to process secret. They must be at least %lu bytes long\n", blockCount * 4);
        return false;
    }
    
    for(size_t i = 1; i < shades->size; i++) {
        if (shades->images[i].size != shadeSize) {
            fprintf(stderr, "Shades are from different sizes.\n");
            return false;
        }
    }

    return true;
}

bool encrypt(uint8_t *secret, size_t size, BMPImagesCollection *shades, uint8_t k, bool padding) {

    // TODO(tobi): Manejar distintos generadores - Llamar a galois_set_generator(generator);
    // TODO(tobi): Agregar padding para manejar cualquier valor de k
    // TODO(tobi): Ponerle tope a k, para que no haya stack overflow
    if(!validate_input(size, shades, k, padding)) {
        return false;
    }
    
    size_t remainder = size % k;
    size_t blockCount = size / k + !!remainder;

    //secret block y xValues se inicializan directamente a pesar de ser variable porque es un uint_8 
    //y no van a ser demasiados bytes
    uint8_t secretBlock[k];
    ShadeBlock shadeBlock;
    uint8_t tValue;
    uint8_t xValues[shades->size];

    for(size_t j = 0; j < blockCount; j++) {
        
        memset(xValues, 0, shades->size);

        // 1. conseguir k pixeles del offset
        if(j != blockCount - 1 || !remainder) {
            memcpy(secretBlock, secret + j*k, k); 
        } else {
            // Agrego padding
            memcpy(secretBlock, secret + j*k, remainder);
            memset(secret + j*k + remainder, 0, k - remainder); 
        }
        
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

uint8_t * decrypt(size_t size, BMPImagesCollection *shades, uint8_t k, bool padding) {
    if(!validate_input(size, shades, k, padding)) {
        return NULL;
    }

    size_t remainder = size % k;
    size_t blockCount = size / k + !!remainder;
    
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

        if(j != blockCount - 1 || !remainder) {
            memcpy(secret + j*k, secretBlock, k);
        } else {
            // No incluye padding
            memcpy(secret + j*k, secretBlock, remainder);
        }
    }

    return secret;
}