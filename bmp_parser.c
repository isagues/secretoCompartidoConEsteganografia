// bmp_parser.c
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <byteswap.h> // Swapear Bigendian a Littleendian

typedef struct BMPFileHeader {
    uint8_t     type[2];
    uint32_t    fileSize;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint32_t    offset;
} BMPFileHeader;

typedef struct BMPInfoHeader {    // bmih
    uint32_t    headerSize;
    uint32_t    width;
    uint32_t    height;
    uint16_t    planes;
    uint16_t    bitCount;
    uint32_t    compression;
    uint32_t    imageSize;
    uint32_t    XPelsPerMeter;
    uint32_t    YPelsPerMeter;
    uint32_t    colorsUsed;
    uint32_t    colorsImportant;
} BMPInfoHeader;

typedef struct BMPImage {
    uint8_t   **data;
    uint64_t    size;
    uint64_t    width;
    uint64_t    height;
} BMPImage;

static void bmp_read_file_header(FILE * fStream, BMPFileHeader *fh);
static void bmp_read_info_header(FILE * fStream, BMPInfoHeader *ih);
static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height);
BMPImage* bmp_read_file(char * path, BMPImage *img);
void bmp_swap_rows(BMPImage *img);

int main() {

    BMPImage image;

    bmp_read_file("lena_gray.bmp", &image);

    printf("Size: %ld\n", image.size);
    printf("Width: %ld\n", image.width);
    printf("Height: %ld\n", image.height);

    bmp_swap_rows(&image);

    for (size_t i = 0; i < image.height; i++){
        printf("%4X", image.data[i][0]);
    }
    
}

BMPImage* bmp_read_file(char *path, BMPImage *img){

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    FILE *fStream = fopen(path, "r");

    if (fStream == NULL) {
        perror("Error openning image file");
        exit(1);
    }

    bmp_read_file_header(fStream, &fileHeader);
    bmp_read_info_header(fStream, &infoHeader);

    img->size = fileHeader.fileSize - fileHeader.offset;
    img->width = infoHeader.width;
    img->height = infoHeader.height;
    img->data = bmp_read_data(fStream, fileHeader.offset, img->width, img->height);

    return img; // Se puede devolver NULL en caso de erorr.
}

void bmp_swap_rows(BMPImage *img) {

    for (int low = 0, high = img->height - 1; low < high; low++, high--) {
        uint8_t *temp = img->data[low];
        img->data[low] = img->data[high];
        img->data[high] = temp;
    }
}

static void bmp_read_file_header(FILE * fStream, BMPFileHeader *fh) {
    
    fread(&fh->type,         sizeof(fh->type),        1,  fStream);
    fread(&fh->fileSize,     sizeof(fh->fileSize),    1,  fStream);
    fread(&fh->reserved1,    sizeof(fh->reserved1),   1,  fStream);
    fread(&fh->reserved2,    sizeof(fh->reserved2),   1,  fStream);
    fread(&fh->offset,       sizeof(fh->offset),      1,  fStream);
}

static void bmp_read_info_header(FILE * fStream, BMPInfoHeader *ih) {
    
    fread(&ih->headerSize,      sizeof(ih->headerSize),     1,  fStream);
    fread(&ih->width,           sizeof(ih->width),          1,  fStream);
    fread(&ih->height,          sizeof(ih->height),         1,  fStream);
    fread(&ih->planes,          sizeof(ih->planes),         1,  fStream);
    fread(&ih->bitCount,        sizeof(ih->bitCount),       1,  fStream);
    fread(&ih->compression,     sizeof(ih->compression),    1,  fStream);
    fread(&ih->imageSize,       sizeof(ih->imageSize),      1,  fStream);
    fread(&ih->XPelsPerMeter,   sizeof(ih->XPelsPerMeter),  1,  fStream);
    fread(&ih->YPelsPerMeter,   sizeof(ih->YPelsPerMeter),  1,  fStream);
    fread(&ih->colorsUsed,      sizeof(ih->colorsUsed),     1,  fStream);
    fread(&ih->colorsImportant, sizeof(ih->colorsImportant),1,  fStream);
}

static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height) {
    
    uint8_t **buff = malloc(height * sizeof(*buff));

    fseek(fStream, offset, SEEK_SET);

    for (size_t i = 0; i < height; i++) {
        uint8_t *ptr = malloc(width * sizeof(*ptr));
        
        size_t bytesRead = fread(ptr, sizeof(*ptr), width, fStream);

        if (bytesRead != width) {
            perror("CABUM");
            exit(1);
        }
        
        buff[i] = ptr;
    }

    return buff;
}
