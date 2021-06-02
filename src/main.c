#include "bmp_parser/bmp_parser.h"
#include "shared_secret/shared_secret.h"
#include "args/args.h"
#include "log/log.h"
#include "galois/galois.h"

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

static int distribute(char * secretPath, uint8_t k, char * shadesPath, char *shadesOutputDir, galois2_8_gen_t galoisGen, bool padding);
static int recover(char * secretPath, uint8_t k, char * shadesPath, galois2_8_gen_t galoisGen, bool padding);

int main(int argc, char *argv[]) {

    Args args;
    if(!args_parse(argc, argv, &args)) {
        LOG_FATAL("Failed to parse args correctly");
        return EXIT_FAILURE;
    }
    
    // Inicializacion logs
    log_set_level(args.loglevel);
    log_set_quiet(args.logQuiet);
    log_set_verbose(args.logVerbose);

    switch(args.action) {
        case DISTRIBUTE:
            return distribute(args.secretImage, args.k, args.shadowsDir, args.shadesOutputDir, args.galoisGen, args.padding);
        
        case RECOVER:
            return recover(args.secretImage, args.k, args.shadowsDir, args.galoisGen, args.padding);

        default:
            LOG_FATAL("Invalid action %c", args.action);
            return EXIT_FAILURE;
    }
}

// TODO(tobi): Manejar logging mejor
static int distribute(char *secretPath, uint8_t k, char *shadesPath, char *shadesOutputDir, galois2_8_gen_t galoisGen, bool padding) {
    
    LOG_INFO("----- Secret Image Distribution -----");

    LOG_INFO("Reading secret image '%s'", secretPath);

    BMPHeader header;
    BMPImage secretImage;
    if(!bmp_read_file(secretPath, &secretImage, &header)) {
        return EXIT_FAILURE;
    }

    LOG_INFO("Reading shades from directory '%s'", shadesPath);

    BMPImagesCollection shades;
    if(!bmp_images_from_directory(shadesPath, &shades, NULL)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        return EXIT_FAILURE;
    }

    for(size_t i = 0; i < shades.size; i++) {
        if(shades.images[i].height != secretImage.height || shades.images[i].width != secretImage.width) {
            LOG_FATAL("Secret image and shades must have all the same dimensions");

            // Rollback
            bmp_header_free(&header);
            bmp_image_free(&secretImage);
            bmp_image_collection_free(&shades);
            return EXIT_FAILURE;
        }
    }

    LOG_INFO("Distributing secret image using scheme (k = %"PRIu8", n = %"PRIu8", g = %"PRIu8")", k, shades.size, galoisGen);

    uint8_t *secret = bmp_image_data(&secretImage);
    
    if(!ss_distribute(secret, secretImage.size, &shades, k, galoisGen, padding)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }
    
    LOG_INFO("Persisting tampered shades in directory '%s'", shadesOutputDir);

    if(!shades_persist(shadesOutputDir, &shades, &header)) {
        // Rollback
        bmp_header_free(&header);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }

    LOG_INFO("Freeing resources...");

    bmp_header_free(&header);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    LOG_INFO("Done!");

    return EXIT_SUCCESS;
}

static int recover(char *secretPath, uint8_t k, char *shadesPath, galois2_8_gen_t galoisGen, bool padding){

    LOG_INFO("----- Secret Image Retrieval -----");

    BMPImage secretImage;

    LOG_INFO("Reading tampered shades from directory '%s'", shadesPath);

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

    LOG_INFO("Recovering secret image using scheme using %"PRIu8" shades of at least %"PRIu8"", shades.size, k);

    uint8_t *secret = ss_recover(secretSize, &shades, k, galoisGen, padding);
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

    LOG_INFO("Persisting secret image at '%s'", secretPath);

    if(!bmp_persist_image(secretPath, &secretImageHeader, &secretImage)) {
        LOG_FATAL("Failed to persist image '%s'", secretPath);
        
        // Rollback
        bmp_header_free(&secretImageHeader);
        bmp_image_free(&secretImage);
        bmp_image_collection_free(&shades);
        return EXIT_FAILURE;
    }

    LOG_INFO("Freeing resources...");

    bmp_header_free(&secretImageHeader);
    bmp_image_free(&secretImage);
    bmp_image_collection_free(&shades);

    LOG_INFO("Done!");

    return EXIT_SUCCESS;
}