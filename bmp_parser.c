#include "bmp_parser.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

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

static void bmp_read_file_header(FILE * fStream, BMPFileHeader *fh);

static void bmp_read_info_header(FILE * fStream, BMPInfoHeader *ih);

static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height);

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

Shades get_images_from_directory(char * directoryPath){
    
    DIR* FD;
    struct dirent* in_file;
    FILE    *common_file;
    FILE    *entry_file;
    Shades shades;
    shades.size = 0;
    shades.images = NULL;

    if (NULL == (FD = opendir (directoryPath))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        fclose(common_file);

        exit(1);
    }

    while ((in_file = readdir(FD))) 
    {
        /* On linux/Unix we don't want current and parent directories
         * On windows machine too, thanks Greg Hewgill
         */
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        if (in_file->d_reclen < sizeof(".bmp") || !strcmp (in_file->d_name + in_file->d_reclen - 5, ".bmp"))    
            continue;

        /* Open directory entry file for common operation */
        /* TODO : change permissions to meet your need! */
        entry_file = fopen(in_file->d_name, "rw");

        if (entry_file == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            fclose(common_file);

            exit(1);
        }
        
        shades.size++;
        realloc(shades.images, sizeof(*shades.images) * (shades.size));
        
        if(bmp_read_file(in_file->d_name, &shades.images[shades.size - 1]) == NULL) {
            printf("Error openning %s", in_file->d_name);
            exit(1);
        }
    
    return shades;
}
