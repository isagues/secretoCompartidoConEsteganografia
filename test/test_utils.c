#include "test_utils.h"

#include <check.h>

#include <inttypes.h>
#include <stdio.h>

void assert_equal_uint8_array(uint8_t expected[], uint8_t actual[], size_t n) {
    for(size_t i = 0; i < n; i++) {
        ck_assert_int_eq(expected[i], actual[i]);
    }
}

void print_uint8_array(uint8_t arr[], size_t n) {
    printf("[");
    for(size_t i = 0; i < n; i++) {
        printf("%" PRIu8 ", ", arr[i]);
    }
    printf("]\n");
}