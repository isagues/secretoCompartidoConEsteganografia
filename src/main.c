#include "bmp_parser.h"
#include "encrypt.h"
#include "arguments.h"

int main(int argc, char *argv[]) {

    Arguments args = args_parse_and_validate(argc, argv);
    // Arguments args = {.action='d', .k=4, .secretImage="images/Gustavo300.bmp", .shadowsDir="images"}; // PA TESTING

    BMPHeader header;
    BMPImage secretImage;

    bmp_read_header(args.secretImage, &header);
    bmp_read_file(args.secretImage, &secretImage);

    BMPImagesCollection initial_shades = get_images_from_directory(args.shadowsDir);

    printf("images loaded\n\n");
    
    for (size_t i = 0; i < initial_shades.images[0].height; i++) {
        printf("%4X", initial_shades.images[0].data[i][0]);
    }

    printf("\n\n");
    uint8_t * secret = bmp_image_to_array(secretImage);
    BMPImagesCollection final_shades = encrypt(secret, secretImage.size, initial_shades, args.k);   
    
    for (size_t i = 0; i < final_shades.images[0].height; i++) {
        printf("%4X", final_shades.images[0].data[i][0]);
    }
    
    persist_new_shades("images", final_shades, header);

    free(secret);
    free(header.data);
    bmp_image_free(secretImage);
    for (size_t i = 0; i < initial_shades.size; i++) {
        bmp_image_free(initial_shades.images[i]);
    }
    free(initial_shades.images);


    //TODO (faus, nacho) hacer los 80 free que nos faltan
}
