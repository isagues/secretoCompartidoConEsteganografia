#include <galois.h>

#include <check.h>
#include "test_utils.h"

#include <stdbool.h>
#include <stdlib.h>

START_TEST (galois_multiplication_test) {
    ck_assert_int_eq(gmul(0x00, 0x00), 0x00);
    ck_assert_int_eq(gmul(0x05, 0xF0), 0x95);
    ck_assert_int_eq(gmul(0x3F, 0xcd), 0x01);
}
END_TEST

START_TEST (galois_inverse_test) {
    ck_assert_int_eq(gmul(ginv(0xAA), 0xAA), GAL_MUL_ID);
    ck_assert_int_eq(gmul(ginv(0xAB), 0xAB), GAL_MUL_ID);
    ck_assert_int_eq(gmul(ginv(0x54), 0x54), GAL_MUL_ID);
    ck_assert_int_eq(gmul(ginv(0x11), 0x11), GAL_MUL_ID);
    ck_assert_int_eq(gmul(ginv(0x91), 0x91), GAL_MUL_ID);
}
END_TEST

START_TEST (galois_division_test) {
    ck_assert_int_eq(gdiv(0xAA, 0xAA), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0xAB, 0xAB), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x54, 0x54), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x11, 0x11), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x91, 0x91), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(247, 19), 89);
}
END_TEST

START_TEST (galois_poly_eval_test) {
    galois2_8_t p[]              = {0x13, 0x75};
    galois2_8_t x[N(p)]          = {0x95, 0x43};
    galois2_8_t expected_y[N(p)] = {27, 228};
    galois2_8_t y[N(p)];

    // Inicializo y
    for(uint8_t i = 0; i < N(y); i++) {
        y[i] = galois_poly_eval(p, N(p), x[i]);
    }

    for(size_t i = 0; i < N(y); i++) {
        ck_assert_int_eq(expected_y[i], y[i]);
    }
}
END_TEST

START_TEST (galois_lagrange_interpolation_test) {
    // They must be unique
    // galois2_8_t p[] = {0x13, 0x75, 0xFA, 0x15, 0x89, 0x72};
    // galois2_8_t x[N(p)] = {0x95, 0x43, 0x93, 0xF8, 0xD2, 0x11};
    galois2_8_t p[] = {0x13, 0x75};
    galois2_8_t x[N(p)] = {0x95, 0x43};
    galois2_8_t y[N(p)];

    // Inicializo y
    for(uint8_t i = 0; i < N(p); i++) {
        y[i] = galois_poly_eval(p, N(p), x[i]);
    }

    galois2_8_t p_ret[N(p)];
    galois_lagrange_interpolation(x, y, p_ret, N(p));

    print_uint8_array(p, N(p));
    print_uint8_array(p_ret, N(p_ret));

    for(size_t i = 0; i < N(p); i++) {
        ck_assert_int_eq(p[i], p_ret[i]);
    }

    for(size_t i = 0; i < N(p); i++) {
        ck_assert_int_eq(y[i], galois_poly_eval(p_ret, N(p_ret), x[i]));
    }
}
END_TEST



Suite * true1_test_suite(void) {

    Suite *s   = suite_create("galois");

    TCase *tc  = tcase_create("operations");
    tcase_add_test(tc, galois_multiplication_test);
    tcase_add_test(tc, galois_inverse_test);
    tcase_add_test(tc, galois_division_test);
    suite_add_tcase(s, tc);

    tc  = tcase_create("polynomials");
    tcase_add_test(tc, galois_lagrange_interpolation_test);
    tcase_add_test(tc, galois_poly_eval_test);
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    SRunner *sr  = srunner_create(true1_test_suite());

    srunner_run_all(sr, CK_NORMAL);

    int number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
