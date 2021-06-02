#include "utils/general_utils.h"

#include <check.h>

#include <stdlib.h>

START_TEST (to_even_test) {
    ck_assert_int_eq(TO_EVEN(3), 2);
    ck_assert_int_eq(TO_EVEN(1111), 1110);
    ck_assert_int_eq(TO_EVEN(99999999999999), 99999999999998);
    ck_assert_int_eq(TO_EVEN(2), 2);
    ck_assert_int_eq(TO_EVEN(0), 0);
    ck_assert_int_eq(TO_EVEN(99999999999998), 99999999999998);
}
END_TEST

Suite * true1_test_suite(void) {

    Suite *s   = suite_create("general_utils");

    TCase *tc  = tcase_create("macros");
    tcase_add_test(tc, to_even_test);
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