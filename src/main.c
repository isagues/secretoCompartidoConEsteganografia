#include "bmp_parser.h"
#include "encrypt.h"
#include "arguments.h"

#include <string.h>

static int distribute(char * secretPath, uint8_t k, char * shadesPath, char *shadesOutputDir);
static int recover(char * secretPath, uint8_t k, char * shadesPath);

int main(int argc, char *argv[]) {

    Arguments args = args_parse_and_validate(argc, argv);
    // Arguments args = {.action='d', .k=4, .secretImage="images/Gustavo300.bmp", .shadowsDir="images"}; // PA TESTING

    switch(args.action) {
        case DISTRIBUTE:
            return distribute(args.secretImage, args.k, args.shadowsDir, args.shadesOutputDir);
        
        case RECOVER:
            return recover(args.secretImage, args.k, args.shadowsDir);

        default:
            return 1;
    }
}

// TODO(tobi): Manejar logging mejor
static int distribute(char *secretPath, uint8_t k, char *shadesPath, char *shadesOutputDir) {
    
    BMPHeader header;
    BMPImage secretImage;
    if(!bmp_read_file(secretPath, &secretImage, &header)) {
        return 2;
    }

    BMPImagesCollection shades;
    if(!bmp_images_from_directory(shadesPath, &shades, NULL)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        return 2;
    }

    uint8_t *secret = bmp_image_data(&secretImage);
    
    if(!encrypt(secret, secretImage.size, &shades, k)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        return 2;
    }
    
    if(shades_persist(shadesOutputDir, &shades, &header)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return 2;
    }

    bmp_header_free(&header);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return 0;
}

static int recover(char *secretPath, uint8_t k, char *shadesPath){

    BMPImage secretImage;

    BMPImagesCollection shades;
    BMPHeader secretImageHeader;
    if(!bmp_images_from_directory(shadesPath, &shades, &secretImageHeader)) {
        return 3;
    }

    if(k <= 0 || shades.size < k) {
        fprintf(stderr, "ERROR");
        return 3;
    }

    size_t secretSize = shades.images[0].height * shades.images[0].width;

    uint8_t *secret = decrypt(secretSize, &shades, k);
    if(secret == NULL) {
        // Rollback
        bmp_header_free(&secretImageHeader);
        bmp_image_collection_free(&shades);
        return 3;
    }

    memcpy(&secretImage, &shades.images[0], sizeof(secretImage));
    
    secretImage.data = malloc(secretImage.height * sizeof(*secretImage.data));
    if(secretImage.data == NULL) {
        // Rollback
        free(secret);
        bmp_header_free(&secretImageHeader);
        bmp_image_collection_free(&shades);
        return 3;
    }
    
    for(size_t i = 0; i < secretImage.height; i++) {
        secretImage.data[i] = secret + secretImage.width * i;
    }

    if(!bmp_persist_image(secretPath, &secretImageHeader, &secretImage)) {
        // Rollback
        bmp_header_free(&secretImageHeader);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return 3;
    }

    bmp_header_free(&secretImageHeader);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return 0;
}