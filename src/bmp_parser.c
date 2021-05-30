#include "bmp_parser.h"

#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

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

bool bmp_read_file(char *path, BMPImage *img, BMPHeader *header) {

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    FILE *fStream = fopen(path, "r");
    if(fStream == NULL) {
        fprintf(stderr, "Error : Failed to open entry file %s - %s\n", path, strerror(errno));
        return false;
    }

    if(
        fread(&fileHeader, sizeof(fileHeader), 1, fStream) < 1 ||
        fread(&infoHeader, sizeof(infoHeader), 1, fStream) < 1
    ) {
        if(feof(fStream)) {
            fprintf(stderr, "Error : Reached EOF while trying to read header from %s\n", path);
        } 
        else {
            fprintf(stderr, "Error : Failed to read header from file %s - %s\n", path, strerror(errno));
        }

        // Rollback
        fclose(fStream);
        return false;
    }

    uint32_t headerSurplus = fileHeader.offset - sizeof(fileHeader) - sizeof(infoHeader);

    if(header != NULL) {
        // Extraer header
        header->size = fileHeader.offset;
        header->data = malloc(fileHeader.offset * sizeof(*header->data));
        uint8_t *dataPtr = (uint8_t *) header->data;

        memcpy(dataPtr, &fileHeader, sizeof(fileHeader));
        dataPtr += sizeof(fileHeader);

        memcpy(dataPtr, &infoHeader, sizeof(infoHeader));
        dataPtr += sizeof(infoHeader);

        if(fread(dataPtr, headerSurplus, 1, fStream) < 1) {
            if(feof(fStream)) {
                fprintf(stderr, "Error : Reached EOF while trying to read header from %s\n", path);
            } 
            else {
                fprintf(stderr, "Error : Failed to read header from file %s - %s\n", path, strerror(errno));
            }

            // Rollback
            free(header->data);
            fclose(fStream);
            return false;
        }
    }
    
    img->size = fileHeader.fileSize - fileHeader.offset;
    img->width = infoHeader.width;
    img->height = infoHeader.height;
    img->data = bmp_read_data(fStream, fileHeader.offset, img->width, img->height);
    if(img->data == NULL) {
        //Rollback
        free(header->data);
        fclose(fStream);
        return false;
    }

    fclose(fStream);
    return true;
}

void bmp_swap_rows(BMPImage *img) {

    for(int low = 0, high = img->height - 1; low < high; low++, high--) {
        uint8_t *temp = img->data[low];
        img->data[low] = img->data[high];
        img->data[high] = temp;
    }
}

static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height) {
    // Pongo el puntero del archivo en el lugar correcto
    if(fseek(fStream, offset, SEEK_SET)) {
        fprintf(stderr, "Error : Failed to read image - %s\n", strerror(errno));
        return NULL;
    }

    // Dynamically allocate multidimensional array: http://c-faq.com/aryptr/dynmuldimary.html
    uint8_t **data = malloc(height * sizeof(*data));
    if(data == NULL) {
        return NULL;
    }
    
    // Aloco toda la memoria contigua con un esquema de doble indireccion
    data[0] = malloc(height * width * sizeof(sizeof(*data[0])));
    if(data[0] == NULL) {
        free(data);
        return NULL;
    }

    // Levanto la imagen a memoria
    if(fread(data[0], height * width * sizeof(*data[0]), 1, fStream) < 1) {
        if(feof(fStream)) {
            // TODO(tobi): Analizar caso
            fprintf(stderr, "Error : Reached EOF while trying to read image\n");
        } 
        else {
            fprintf(stderr, "Error : Failed to read image - %s\n", strerror(errno));
        }

        free(data[0]);
        free(data);
        return NULL;
    }

    // Distribuyo los punteros
    for(size_t i = 0; i < height; i++) {
        data[i] = data[0] + i * width;
    }

    return data;
}

uint8_t * bmp_image_data(BMPImage *image) {
    return image->data[0];
}

// source: https://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c/11737506#11737506
bool bmp_images_from_directory(char * directoryPath, BMPImagesCollection *imagesCollection, BMPHeader *sampleHeader) {
    
    DIR* FD;
    struct dirent* in_file;
    BMPImage *tmpImagePtr = NULL;

    imagesCollection->size = 0;
    imagesCollection->images = NULL;

    char filename[PATH_MAX];

    BMPHeader *tmpHeader = sampleHeader;

    if(NULL == (FD = opendir (directoryPath))) {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return false;
    }

    while((in_file = readdir(FD))) {

        if(!strcmp (in_file->d_name, "."))
            continue;
        if(!strcmp (in_file->d_name, ".."))    
            continue;
        if(in_file->d_reclen < sizeof(".bmp") || !strcmp (in_file->d_name + in_file->d_reclen - 5, ".bmp"))    
            continue;

        sprintf(filename, "%s/%s", directoryPath, in_file->d_name);
        imagesCollection->size++;
        
        if((tmpImagePtr = realloc(imagesCollection->images, sizeof(*imagesCollection->images) * (imagesCollection->size))) == NULL){
            printf("Not enough space for images");

            // Rollback
            imagesCollection->size--;
            bmp_image_collection_free(imagesCollection);
            closedir(FD);
            return false;
        }
        imagesCollection->images = tmpImagePtr;
        
        if(!bmp_read_file(filename, &imagesCollection->images[imagesCollection->size - 1], tmpHeader)) {
            printf("Error openning %s", in_file->d_name);

            // Rollback
            free(&imagesCollection->images[imagesCollection->size - 1]);
            imagesCollection->size--;
            bmp_image_collection_free(imagesCollection);
            closedir(FD);
            return false;
        }
        if(tmpHeader != NULL) {
            // La primera vez (si no es null) le carga la info al header, y luego el resto de las veces en NULL (no hace nada).
            tmpHeader = NULL;
        }
    }

    closedir(FD);
    return true;
}

void bmp_image_collection_free(BMPImagesCollection *collection) {
    for(size_t i = 0; i < collection->size; i++) {
        bmp_image_free(&collection->images[i]);
    }
    free(collection->images);
}

void bmp_image_free(BMPImage *image) {
    free(image->data[0]);
    free(image->data);
}

void bmp_header_free(BMPHeader *header) {
    free(header->data);
}

bool bmp_persist_image(char * auxPath, BMPHeader *header, BMPImage *image) {

    //TODO (faus, nacho) se estan creando nuevas shades pero no se pisan
    FILE *fStream = fopen(auxPath, "w+");
    if(fStream == NULL) {
        fprintf(stderr, "Error : Failed to open entry file %s - %s\n", auxPath, strerror(errno));
        return false;
    }

    if(
        fwrite(header->data,    header->size,                   1, fStream) < 1 ||
        fwrite(image->data[0],  image->height * image->width,   1, fStream) < 1
    ) {
        fprintf(stderr, "Error : Failed to persist image to file %s - %s\n", auxPath, strerror(errno));
        fclose(fStream);
        return false;
    }

    fclose(fStream);
    return true;
}