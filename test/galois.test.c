#include "galois/galois.h"
#include "utils/general_utils.h"

#include <check.h>
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
    ck_assert_int_eq(gmul(ginv(79)  , 79),   GAL_MUL_ID);
    ck_assert_int_eq(gmul(ginv(125) , 125),  GAL_MUL_ID);
}
END_TEST

START_TEST (galois_division_test) {
    ck_assert_int_eq(gdiv(0xAA, 0xAA), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0xAB, 0xAB), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x54, 0x54), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x11, 0x11), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(0x91, 0x91), GAL_MUL_ID);
    ck_assert_int_eq(gdiv(247, 19), 89);
    ck_assert_int_eq(gdiv(79, 125), 149);
}
END_TEST

START_TEST (galois_poly_eval_test) {
    {
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

    {
        galois2_8_t p[]              = {193, 174, 186, 182, 171};
        galois2_8_t x[N(p)]          = {194, 193, 11, 50, 79};
        galois2_8_t expected_y[N(p)] = {166, 80, 140, 41, 83};
        galois2_8_t y[N(p)];

        // Inicializo y
        for(uint8_t i = 0; i < N(y); i++) {
            y[i] = galois_poly_eval(p, N(p), x[i]);
        }

        for(size_t i = 0; i < N(y); i++) {
            ck_assert_int_eq(expected_y[i], y[i]);
        }
    }
}
END_TEST

START_TEST (galois_lagrange_interpolation_test) {

    {
        galois2_8_t p[]     = {193, 174, 186, 182, 171};
        galois2_8_t x[N(p)] = {194, 193, 11,  50,  79};
        galois2_8_t y[N(p)];

        // Inicializo y
        for(uint8_t i = 0; i < N(p); i++) {
            y[i] = galois_poly_eval(p, N(p), x[i]);
        }

        galois2_8_t p_ret[N(p)];
        galois_lagrange_interpolation(x, y, p_ret, N(p));

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(p[i], p_ret[i]);
        }

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(y[i], galois_poly_eval(p_ret, N(p_ret), x[i]));
        }
    }

    {
        // Base test
        galois2_8_t p[] = {0x13, 0x75, 0xFA, 0x15, 0x89};
        galois2_8_t x[N(p)] = {0x95, 0x43, 0x93, 0xF8, 0xD2};
        galois2_8_t y[N(p)];

        // Inicializo y
        for(uint8_t i = 0; i < N(p); i++) {
            y[i] = galois_poly_eval(p, N(p), x[i]);
        }

        galois2_8_t p_ret[N(p)];
        galois_lagrange_interpolation(x, y, p_ret, N(p));

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(p[i], p_ret[i]);
        }

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(y[i], galois_poly_eval(p_ret, N(p_ret), x[i]));
        }
    }

    {
        // test with null x last
        galois2_8_t p[] = {0x13, 0x75, 0xFA, 0x15, 0x89};
        galois2_8_t x[N(p)] = {0x95, 0x43, 0x93, 0xF8, 0x00};
        galois2_8_t y[N(p)];

        // Inicializo y
        for(uint8_t i = 0; i < N(p); i++) {
            y[i] = galois_poly_eval(p, N(p), x[i]);
        }

        galois2_8_t p_ret[N(p)];
        galois_lagrange_interpolation(x, y, p_ret, N(p));

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(p[i], p_ret[i]);
        }

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(y[i], galois_poly_eval(p_ret, N(p_ret), x[i]));
        }
    }

    {
        // test with null x in the middle
        galois2_8_t p[] = {0x13, 0x75, 0xFA, 0x15, 0x89};
        galois2_8_t x[N(p)] = {0x95, 0x43, 0x00, 0xF8, 0xD2};
        galois2_8_t y[N(p)];

        // Inicializo y
        for(uint8_t i = 0; i < N(p); i++) {
            y[i] = galois_poly_eval(p, N(p), x[i]);
        }

        galois2_8_t p_ret[N(p)];
        galois_lagrange_interpolation(x, y, p_ret, N(p));

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(p[i], p_ret[i]);
        }

        for(size_t i = 0; i < N(p); i++) {
            ck_assert_int_eq(y[i], galois_poly_eval(p_ret, N(p_ret), x[i]));
        }
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
