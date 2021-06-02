#include "shared_secret/shared_secret.h"

#include "shared_secret/shades.h"
#include "log/log.h"
#include "utils/general_utils.h"

#include <string.h>
#include <inttypes.h>

static bool validate_input(size_t secretSize, BMPImagesCollection *shades, uint8_t k, bool padding);

uint8_t encrypt_function(uint8_t * secretBlock, size_t k, uint8_t x) {
    return galois_poly_eval(secretBlock, k, x);    // s1 + s2*X + ... +sk*X^k-1 mod g(x)
}

static bool validate_input(size_t secretSize, BMPImagesCollection *shades, uint8_t k, bool padding) {
    
    if(k < 1) {
        LOG_FATAL("k must be at least 2");
        return false;
    }

    size_t remainder = secretSize % k;
    size_t blockCount = secretSize / k + !!remainder;

    if(remainder && !padding) {
        LOG_FATAL("Secret must be a divisible by k or padding must be enabled");
        return false;
    } 
    
    if(shades->size < k) {
        LOG_FATAL("Not enough shades to process secret were provided. %d shades are needed.", k);
        return false;
    }

    size_t shadeSize = shades->images[0].size;

    for(size_t i = 0; i < shades->size; i++) {
        size_t shadeBlockCount = TO_EVEN(shades->images[i].height) * TO_EVEN(shades->images[i].width) / SHADE_BLOCK_SIZE;
        if(blockCount > shadeBlockCount) {
            LOG_FATAL("Shades are not big enough to process secret. They must be at least %lu bytes long\n", blockCount * 4);
            return false;
        }
    }
    
    return true;
}

bool ss_distribute(uint8_t *secret, size_t size, BMPImagesCollection *shades, uint8_t k, galois2_8_gen_t galoisGen, bool padding) {

    if(!validate_input(size, shades, k, padding)) {
        return false;
    }

    if(!galois_set_generator(galoisGen)) {
        LOG_FATAL("Invalid galois generator '%"PRIu16"' configured for secret distribution", galoisGen);
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
        snprintf(auxPath, PATH_MAX - 1,"%s/shade_%ld.bmp", dirPath, i);
        ret &= bmp_persist_image(auxPath, header, &shades->images[i]);
    }

    return ret;
}

uint8_t * ss_recover(size_t size, BMPImagesCollection *shades, uint8_t k, galois2_8_gen_t galoisGen, bool padding) {
    if(!validate_input(size, shades, k, padding)) {
        return NULL;
    }

    if(!galois_set_generator(galoisGen)) {
        LOG_FATAL("Invalid galois generator '%"PRIu16"' configured for secret recovery", galoisGen);
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