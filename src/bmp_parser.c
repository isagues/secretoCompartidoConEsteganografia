#include "bmp_parser.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#define PATH_MAX 4096

//TODO(nacho, faus): Por ahora esta con packing. Habria que ver cuando funciona y si son los casos de uso esperados.
typedef struct BMPFileHeader {
    uint8_t     type[2];
    uint32_t    fileSize;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint32_t    offset;
} __attribute__((packed)) BMPFileHeader;

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
} __attribute__((packed)) BMPInfoHeader;

static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height);

BMPHeader* bmp_read_header(char *path, BMPHeader* header){
    
    FILE *fStream = fopen(path, "r");

    if (fStream == NULL) {
        fprintf(stderr, "Error : Failed to open entry file %s - %s\n", path, strerror(errno));
        fclose(fStream);

        exit(1);
    }
        

    if (fStream == NULL) {
        perror("Error openning image file");
        exit(1);
    }

    BMPFileHeader fileHeader;
    fread(&fileHeader, sizeof(fileHeader), 1, fStream);

    header->data = malloc(fileHeader.offset);

    fseek(fStream, 0, SEEK_SET);

    if(fread(header->data, 1, fileHeader.offset, fStream) != fileHeader.offset) {
        printf("Could not read file's %s header", path);
        exit(1);
    }
    header->size = fileHeader.offset;
    fclose(fStream);

    return header;
}

BMPImage* bmp_read_file(char *path, BMPImage *img){

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    FILE *fStream = fopen(path, "r");

    if (fStream == NULL) {
        fprintf(stderr, "Error : Failed to open entry file %s - %s\n", path, strerror(errno));
        fclose(fStream);

        exit(1);
    }
        
    if (fStream == NULL) {
        perror("Error openning image file");
        exit(1);
    }

    fread(&fileHeader, sizeof(fileHeader), 1, fStream);
    fread(&infoHeader, sizeof(infoHeader), 1, fStream);
    
    img->size = fileHeader.fileSize - fileHeader.offset;
    img->width = infoHeader.width;
    img->height = infoHeader.height;
    img->data = bmp_read_data(fStream, fileHeader.offset, img->width, img->height);

    fclose(fStream);

    return img; // Se puede devolver NULL en caso de erorr.
}

void bmp_swap_rows(BMPImage *img) {

    for (int low = 0, high = img->height - 1; low < high; low++, high--) {
        uint8_t *temp = img->data[low];
        img->data[low] = img->data[high];
        img->data[high] = temp;
    }
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

uint8_t * bmp_image_to_array(BMPImage image){
    uint8_t * data_array = malloc(image.size * sizeof(uint8_t));    

    for (size_t i = 0; i < image.height; i++) {
        memcpy(data_array + i*image.width, image.data[i], image.width);
        
    }
    return data_array;
}

// source: https://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c/11737506#11737506
BMPImagesCollection get_images_from_directory(char * directoryPath){
    
    DIR* FD;
    struct dirent* in_file;
    BMPImagesCollection imagesCollection;

    imagesCollection.size = 0;
    imagesCollection.images = NULL;

    char filename[PATH_MAX];

    if (NULL == (FD = opendir (directoryPath))) {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));

        exit(1);
    }

    while ((in_file = readdir(FD))) {

        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        if (in_file->d_reclen < sizeof(".bmp") || !strcmp (in_file->d_name + in_file->d_reclen - 5, ".bmp"))    
            continue;

        sprintf(filename, "%s/%s", directoryPath, in_file->d_name);
        imagesCollection.size++;
        
        if((imagesCollection.images = realloc(imagesCollection.images, sizeof(*imagesCollection.images) * (imagesCollection.size))) == NULL){
            printf("Not enough space for images");
            exit(1);  
        }
        
        if(bmp_read_file(filename, &imagesCollection.images[imagesCollection.size - 1]) == NULL) {
            printf("Error openning %s", in_file->d_name);
            exit(1);
        }
    }

    closedir(FD);
    return imagesCollection;
}

BMPHeader get_sample_header_from_directory(char * directoryPath) {
    
    DIR* FD;
    struct dirent* in_file;

    BMPHeader header; 

    char filename[PATH_MAX];

    if (NULL == (FD = opendir (directoryPath))) {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));

        exit(1);
    }

    while ((in_file = readdir(FD))) {

        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        if (in_file->d_reclen < sizeof(".bmp") || !strcmp (in_file->d_name + in_file->d_reclen - 5, ".bmp"))    
            continue;

        sprintf(filename, "%s/%s", directoryPath, in_file->d_name);
        
        bmp_read_header(filename, &header);
        closedir(FD);
        return header;
    }

    exit(1);
}

void bmp_image_free(BMPImage image) {

    for (size_t i = 0; i < image.height; i++) {
        free(image.data[i]);
    }
    free(image.data);

}

void persist_bmp_image(char * auxPath, BMPHeader header, BMPImage image){

    //TODO (faus, nacho) se estan creando nuevas shades pero no se pisan
    FILE *fStream = fopen(auxPath, "w+");
    
    if (fStream == NULL) {
        fprintf(stderr, "Error : Failed to open entry file %s - %s\n", auxPath, strerror(errno));
        fclose(fStream);

        exit(1);
    }
        

    if (fStream == NULL) {
        perror("Error openning image file");
        exit(1);
    }

    fwrite(header.data, 1, header.size, fStream);
    
    for (size_t i = 0; i < image.height; i++)
    {
        fwrite(image.data[i], 1, image.width, fStream);
    }

    fclose(fStream);
}