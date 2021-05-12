#ifndef __BMP__PARSER__
#define __BMP__PARSER__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <byteswap.h> // Swapear Bigendian a Littleendian

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

uint8_t * bmp_image_to_array(BMPImage image);

BMPHeader* bmp_read_header(char *path, BMPHeader* header);

BMPImage* bmp_read_file(char * path, BMPImage *img);

void bmp_swap_rows(BMPImage *img);

BMPImagesCollection get_images_from_directory(char * directoryPath);

void persist_bmp_image(char * auxPath, BMPHeader header, BMPImage image);

#endif