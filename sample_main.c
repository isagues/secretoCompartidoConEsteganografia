#include "bmp_parser.h"
#include "encrypt.h"

int main() {

    BMPImage image;

    bmp_read_file("images/Gustavo300.bmp", &image);

    printf("Size: %ld\n", image.size);
    printf("Width: %ld\n", image.width);
    printf("Height: %ld\n", image.height);

    BMPImagesCollection initial_shades = get_images_from_directory("images");
    printf("images loaded\n\n");
    for (size_t i = 0; i < initial_shades.images[0].height; i++)
    {
        printf("%4X", initial_shades.images[0].data[i][0]);
    }
    
    int k = 4;
    printf("\n\n");
    BMPImagesCollection final_shades = encrypt(bmp_image_to_array(image), image.size, initial_shades, k);   
    
    for (size_t i = 0; i < final_shades.images[0].height; i++)
    {
        printf("%4X", final_shades.images[0].data[i][0]);
    }
}