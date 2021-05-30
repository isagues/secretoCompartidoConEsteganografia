#include "general_utils.h"

#include <inttypes.h>
#include <stdio.h>

void print_uint8_array(uint8_t arr[], size_t n) {
    printf("[");
    for(size_t i = 0; i < n; i++) {
        printf("%" PRIu8 ", ", arr[i]);
    }
    printf("]\n");
}