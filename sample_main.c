#include "bmp_parser.h"
#include "encrypt.h"

int main() {

    BMPImage image;

    bmp_read_file("images/lena_gray.bmp", &image);

    printf("Size: %ld\n", image.size);
    printf("Width: %ld\n", image.width);
    printf("Height: %ld\n", image.height);

    bmp_swap_rows(&image);

    for (size_t i = 0; i < image.height; i++){
        printf("%4X", image.data[i][0]);
    }

    Shades initial_shades = get_images_from_directory("images");
    int k = 4;
    Shades final = encrypt_data(image.data, image.size, initial_shades, k);   
    
}