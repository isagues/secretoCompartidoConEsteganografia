#include <encrypt.h>

#include <string.h>

uint8_t encrypt_function(uint8_t * secretBlock, size_t k, uint8_t x){
    return galois_poly_eval(secretBlock, k, x);    // s1 + s2*X + ... +sk*X^k-1 mod g(x)
}

BMPImagesCollection encrypt(uint8_t * secret, size_t size, BMPImagesCollection initial_shades, uint8_t k) {

    //TODO(tobi): Manejar distintos generadores - Llamar a galois_set_generator(generator);

    // TODO(tobi): Agregar padding para manejar cualquier valor de k
    if (k == 0 || k > 256 || (size % k) != 0){
        printf("invalid value for k = %c\n", k);
        exit(1);
    }
    
    size_t blockCount = size / k;
    
    if(initial_shades.size < k || blockCount > initial_shades.images[0].size/4 ){
        printf("invalid parameters\n");
        exit(1);
    }
    
    size_t shade_size = initial_shades.images[0].size;
    for (size_t i = 1; i < initial_shades.size; i++) {
        if (initial_shades.images[i].size != shade_size) {
            perror("Shades are from different sizes.\n");
            exit(1);
        }
    }

    uint8_t * secretBlock = malloc(k*sizeof(uint8_t));
    ShadeBlock shadeBlock;
    uint8_t auxT;
    uint8_t *xValues = malloc(initial_shades.size*sizeof(uint8_t));

    for (size_t j = 0; j < blockCount; j++) {
        
        memset(xValues, 0, initial_shades.size);

        memcpy(secretBlock, secret + j*k, k); //1 conseguir k pixeles del offset
        
        // Indices de esta forma para poder seguir con la convencion del paper
        for (size_t i = 0; i < initial_shades.size; i++) {
            shadeBlock = get_shadeblock_from_index(initial_shades.images[i], j); //2 conseguir las SHADES_NUMBER matrices de 2*2
            
            shadeBlock = guarantee_different_x_values(shadeBlock, xValues, i);

            xValues[i] = shadeBlock.x;

            auxT = encrypt_function(secretBlock, k, shadeBlock.x); // s1 + s2*X + ... +sk*X^k-1 mod g(x)
            
            
            //calcular nuevos valores del shadeBlock
            shadeBlock = distribute_t_value(shadeBlock, auxT);

            set_shadeblock_from_index(initial_shades.images[i], j, shadeBlock);

        }  
    }
    return initial_shades;
}