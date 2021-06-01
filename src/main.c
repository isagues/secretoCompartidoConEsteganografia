#include "bmp_parser/bmp_parser.h"
#include "shared_secret/shared_secret.h"
#include "args/args.h"
#include "log/log.h"

#include <string.h>
#include <stdlib.h>

static int distribute(char * secretPath, uint8_t k, char * shadesPath, char *shadesOutputDir, bool padding);
static int recover(char * secretPath, uint8_t k, char * shadesPath, bool padding);

int main(int argc, char *argv[]) {

    Args args;
    
    if(!args_parse(argc, argv, &args)) {
        LOG_FATAL("Failed to parse args correctly.");
        return EXIT_FAILURE;
    }
    // Arguments args = {.action='d', .k=4, .secretImage="images/Gustavo300.bmp", .shadowsDir="images"}; // PA TESTING

    switch(args.action) {
        case DISTRIBUTE:
            return distribute(args.secretImage, args.k, args.shadowsDir, args.shadesOutputDir, args.padding);
        
        case RECOVER:
            return recover(args.secretImage, args.k, args.shadowsDir, args.padding);

        default:
            LOG_FATAL("Invalid action was recieved");
            return EXIT_FAILURE;
    }
}

// TODO(tobi): Manejar logging mejor
static int distribute(char *secretPath, uint8_t k, char *shadesPath, char *shadesOutputDir, bool padding) {
    
    LOG_INFO("Distributing secret: %s creating shades using images from: %s. Shades in: %s.", secretPath, shadesPath, shadesOutputDir);

    BMPHeader header;
    BMPImage secretImage;
    if(!bmp_read_file(secretPath, &secretImage, &header)) {
        return EXIT_FAILURE;
    }

    BMPImagesCollection shades;
    if(!bmp_images_from_directory(shadesPath, &shades, NULL)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        return EXIT_FAILURE;
    }

    LOG_INFO("Using scheme (%d, %d).", k, shades.size);

    uint8_t *secret = bmp_image_data(&secretImage);
    
    if(!ss_distribute(secret, secretImage.size, &shades, k, padding)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }
    
    if(!shades_persist(shadesOutputDir, &shades, &header)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }

    bmp_header_free(&header);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return EXIT_SUCCESS;
}

static int recover(char *secretPath, uint8_t k, char *shadesPath, bool padding){

    LOG_INFO("Recovering secret from shades: %s. Recovered secret: %s",  shadesPath, secretPath);

    BMPImage secretImage;

    BMPImagesCollection shades;
    BMPHeader secretImageHeader;
    if(!bmp_images_from_directory(shadesPath, &shades, &secretImageHeader)) {
        return EXIT_FAILURE;
    }

    if(shades.size == 0) {
        LOG_FATAL("No .bmp images were found on directory '%s'", shadesPath);
        return EXIT_FAILURE;
    }

    size_t secretSize = shades.images[0].height * shades.images[0].width;

    uint8_t *secret = ss_recover(secretSize, &shades, k, padding);
    if(secret == NULL) {
        // Rollback
        bmp_header_free(&secretImageHeader);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }

    memcpy(&secretImage, &shades.images[0], sizeof(secretImage));
    
    secretImage.data = malloc(secretImage.height * sizeof(*secretImage.data));
    if(secretImage.data == NULL) {
        LOG_FATAL("Failed to allocate memory for secret data");

        // Rollback
        free(secret);
        bmp_header_free(&secretImageHeader);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }
    
    for(size_t i = 0; i < secretImage.height; i++) {
        secretImage.data[i] = secret + secretImage.width * i;
    }

    if(!bmp_persist_image(secretPath, &secretImageHeader, &secretImage)) {
        LOG_FATAL("Failed to persist image '%s'", secretPath);
        
        // Rollback
        bmp_header_free(&secretImageHeader);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }

    bmp_header_free(&secretImageHeader);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    return EXIT_SUCCESS;
}