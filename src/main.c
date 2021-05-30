#include "bmp_parser.h"
#include "encrypt.h"
#include "arguments.h"
#include <string.h>

void distribute(char * secretPath, uint8_t k, char * shadesPath);
void recover(char * secretPath, uint8_t k, char * shadesPath);

int main(int argc, char *argv[]) {

    Arguments args = args_parse_and_validate(argc, argv);
    // Arguments args = {.action='d', .k=4, .secretImage="images/Gustavo300.bmp", .shadowsDir="images"}; // PA TESTING

    switch (args.action) {
        case DISTRIBUTE:
            distribute(args.secretImage, args.k, args.shadowsDir);
            break;
        
        case RECOVER:
            recover(args.secretImage, args.k, args.shadowsDir);
            break;

        default:
            break;
    }
}

void distribute(char * secretPath, uint8_t k, char * shadesPath) {
    
    BMPHeader header;
    BMPImage secretImage;

    bmp_read_header(secretPath, &header);
    bmp_read_file(secretPath, &secretImage);

    BMPImagesCollection initial_shades = get_images_from_directory(shadesPath);

    uint8_t * secret = bmp_image_to_array(secretImage);
    BMPImagesCollection final_shades = encrypt(secret, secretImage.size, initial_shades, k);   
    
    persist_new_shades("images", final_shades, header);

    free(secret);
    free(header.data);
    bmp_image_free(secretImage);
    for (size_t i = 0; i < initial_shades.size; i++) {
        bmp_image_free(initial_shades.images[i]);
    }
    free(initial_shades.images);
}

void recover(char * secretPath, uint8_t k, char * shadesPath){

    BMPImagesCollection shades = get_images_from_directory(shadesPath);

    if(k <= 0 || shades.size < k) {
        fprintf(stderr, "ERROR");
        exit(1);
    }

    size_t secretSize = shades.images[0].height * shades.images[0].width;

    uint8_t *secret = decrypt(secretSize, shades, k);
    BMPImage secretImage;
    BMPHeader secretImageHeader = get_sample_header_from_directory(shadesPath);

    memcpy(&secretImage, &shades.images[0], sizeof(secretImage));
    
    secretImage.data = malloc(secretImage.height * sizeof(*secretImage.data));
    
    for(size_t i = 0; i < secretImage.height; i++) {
        secretImage.data[i] = secret + secretImage.width * i;
    }

    persist_bmp_image(secretPath, secretImageHeader, secretImage);

    free(secret);
    free(secretImage.data);
    free(secretImageHeader.data);
    for (size_t i = 0; i < shades.size; i++) {
        bmp_image_free(shades.images[i]);
    }
    free(shades.images);
}