#include "bmp_parser.h"
#include "encrypt.h"
#include "arguments.h"

#include <string.h>

static int distribute(char * secretPath, uint8_t k, char * shadesPath, char *shadesOutputDir);
static int recover(char * secretPath, uint8_t k, char * shadesPath);

int main(int argc, char *argv[]) {

    Arguments args = args_parse_and_validate(argc, argv);
    // Arguments args = {.action='d', .k=4, .secretImage="images/Gustavo300.bmp", .shadowsDir="images"}; // PA TESTING

    switch (args.action) {
        case DISTRIBUTE:
            return distribute(args.secretImage, args.k, args.shadowsDir, args.shadesOutputDir);
        
        case RECOVER:
            return recover(args.secretImage, args.k, args.shadowsDir);

        default:
            return 1;
    }
}

static int distribute(char *secretPath, uint8_t k, char *shadesPath, char *shadesOutputDir) {
    
    BMPHeader header;
    BMPImage secretImage;
    bmp_read_file(secretPath, &secretImage, &header);

    BMPImagesCollection shades;
    bmp_images_from_directory(shadesPath, &shades, NULL);

    uint8_t *secret = bmp_image_data(&secretImage);
    encrypt(secret, secretImage.size, &shades, k);   
    
    shades_persist(shadesOutputDir, &shades, &header);

    bmp_header_free(&header);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return 0;
}

static int recover(char *secretPath, uint8_t k, char *shadesPath){

    BMPImage secretImage;

    BMPImagesCollection shades;
    BMPHeader secretImageHeader;
    bmp_images_from_directory(shadesPath, &shades, &secretImageHeader);

    if(k <= 0 || shades.size < k) {
        fprintf(stderr, "ERROR");
        exit(1);
    }

    size_t secretSize = shades.images[0].height * shades.images[0].width;

    uint8_t *secret = decrypt(secretSize, &shades, k);

    memcpy(&secretImage, &shades.images[0], sizeof(secretImage));
    
    secretImage.data = malloc(secretImage.height * sizeof(*secretImage.data));
    
    for(size_t i = 0; i < secretImage.height; i++) {
        secretImage.data[i] = secret + secretImage.width * i;
    }

    bmp_persist_image(secretPath, &secretImageHeader, &secretImage);

    bmp_header_free(&secretImageHeader);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return 0;
}