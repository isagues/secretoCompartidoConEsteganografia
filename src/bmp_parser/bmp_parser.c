#include "bmp_parser/bmp_parser.h"
#include "log/log.h"

#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

static const char BMP_EXTENSION[] = ".bmp";
static const size_t BMP_EXTENSION_LEN = sizeof(BMP_EXTENSION);

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
        LOG_FATAL("Failed to open entry file %s - %s", path, strerror(errno));
        return false;
    }

    if(
        fread(&fileHeader, sizeof(fileHeader), 1, fStream) < 1 ||
        fread(&infoHeader, sizeof(infoHeader), 1, fStream) < 1
    ) {
        if(feof(fStream)) {
            LOG_FATAL("Reached EOF while trying to read header from %s", path);
        } 
        else {
            LOG_FATAL("Failed to read header from file %s - %s", path, strerror(errno));
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
                LOG_FATAL("Failed to read header from file %s - %s", path, strerror(errno));
            } 
            else {
                LOG_FATAL("Failed to read header from file %s - %s", path, strerror(errno));
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
        LOG_FATAL("Failed to read header from file %s - %s", path, strerror(errno));
        //Rollback
        free(header->data);
        fclose(fStream);
        return false;
    }

    fclose(fStream);
    return true;
}


static uint8_t** bmp_read_data(FILE * fStream, uint32_t offset, uint32_t width, uint32_t height) {
    // Pongo el puntero del archivo en el lugar correcto
    if(fseek(fStream, offset, SEEK_SET)) {
        LOG_FATAL("Failed to read image - %s\n", strerror(errno));
        return NULL;
    }

    // Dynamically allocate multidimensional array: http://c-faq.com/aryptr/dynmuldimary.html
    uint8_t **data = malloc(height * sizeof(*data));
    if(data == NULL) {
        LOG_FATAL("Failed to allocate memory for data - %s", strerror(errno));
        return NULL;
    }
    
    // Aloco toda la memoria contigua con un esquema de doble indireccion
    data[0] = malloc(height * width * sizeof(*data[0]));
    if(data[0] == NULL) {
        LOG_FATAL("Failed to allocate memory for data - %s", strerror(errno));
        free(data);
        return NULL;
    }

    // Levanto la imagen a memoria
    if(fread(data[0], height * width * sizeof(*data[0]), 1, fStream) < 1) {
        if(feof(fStream)) {
            // TODO(tobi): Analizar caso
            LOG_FATAL("Reached EOF while trying to read image");
        } 
        else {
            LOG_FATAL("Failed to read image - %s", strerror(errno));
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
        LOG_FATAL("Failed to open input directory - %s\n", strerror(errno));
        return false;
    }

    while((in_file = readdir(FD))) {

        size_t pathLen = strlen(in_file->d_name);
        if(pathLen < BMP_EXTENSION_LEN + 1 || strcmp(in_file->d_name + pathLen - BMP_EXTENSION_LEN + 1, BMP_EXTENSION))
            continue;

        snprintf(filename, PATH_MAX - 1, "%s/%s", directoryPath, in_file->d_name);
        imagesCollection->size++;
        
        if((tmpImagePtr = realloc(imagesCollection->images, sizeof(*imagesCollection->images) * (imagesCollection->size))) == NULL){
            LOG_FATAL("Failed to allocate memory for image container");

            // Rollback
            imagesCollection->size--;
            bmp_image_collection_free(imagesCollection);
            closedir(FD);
            return false;
        }
        imagesCollection->images = tmpImagePtr;
        
        if(!bmp_read_file(filename, &imagesCollection->images[imagesCollection->size - 1], tmpHeader)) {
            LOG_FATAL("Error openning file %s", filename);

            // Rollback
            free(&imagesCollection->images[imagesCollection->size - 1]);
            imagesCollection->size--;
            bmp_image_collection_free(imagesCollection);
            closedir(FD);
            return false;
        }
        // La primera vez (si no es null) le carga la info al header, y luego el resto de las veces en NULL (no hace nada).
        tmpHeader = NULL;
        
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
        LOG_FATAL("Failed to open entry file %s - %s\n", auxPath, strerror(errno));

        return false;
    }

    if(
        fwrite(header->data,    header->size,                   1, fStream) < 1 ||
        fwrite(image->data[0],  image->height * image->width,   1, fStream) < 1
    ) {
        LOG_FATAL("Failed to persist image to file %s - %s\n", auxPath, strerror(errno));
        fclose(fStream);
        return false;
    }

    fclose(fStream);
    return true;
}