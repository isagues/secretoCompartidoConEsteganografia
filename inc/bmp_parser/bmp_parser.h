#ifndef __BMP__PARSER__
#define __BMP__PARSER__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <byteswap.h> // Swapear Bigendian a Littleendian

#define PATH_MAX 4096

typedef struct BMPHeader {
    uint8_t *data;
    size_t size;
} BMPHeader;

typedef struct BMPImage {
    uint8_t   **data;
    uint64_t    size;
    uint64_t    width;
    uint64_t    height;
} BMPImage;

typedef struct BMPImagesCollection {
    uint8_t size;
    BMPImage *images;
} BMPImagesCollection;

uint8_t * bmp_image_data(BMPImage *image);

bool bmp_read_file(char * path, BMPImage *img, BMPHeader *header);

bool bmp_images_from_directory(char * directoryPath, BMPImagesCollection *imagesCollection, BMPHeader *sampleHeader);

void bmp_image_collection_free(BMPImagesCollection *collection);

void bmp_image_free(BMPImage *image);

void bmp_header_free(BMPHeader *header);

bool bmp_persist_image(char * auxPath, BMPHeader *header, BMPImage *image);

#endif