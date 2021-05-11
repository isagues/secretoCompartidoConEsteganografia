#include <check.h>

#include <stdbool.h>
#include <stdlib.h>

START_TEST (true_test) {
    ck_assert(true);
}
END_TEST

Suite * true1_test_suite(void) {

    Suite *s   = suite_create("true1");
    TCase *tc  = tcase_create("true");

    tcase_add_test(tc, true_test);

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
