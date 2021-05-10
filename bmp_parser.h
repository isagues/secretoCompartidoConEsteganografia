#ifndef __BMP__PARSER__
#define __BMP__PARSER__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <byteswap.h> // Swapear Bigendian a Littleendian

typedef struct BMPImage {
    uint8_t   **data;
    uint64_t    size;
    uint64_t    width;
    uint64_t    height;
} BMPImage;

typedef struct Shades {
    uint8_t size;
    BMPImage * images;
} Shades;

BMPImage* bmp_read_file(char * path, BMPImage *img);

void bmp_swap_rows(BMPImage *img);

Shades get_images_from_directory(char * directoryPath);

#endif