#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_is_equal_1) {
    s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 80000000}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 80000000}};
    int return_value = s21_is_equal(value_1, value_2);
    ck_assert_int_eq(return_value, TRUE);
}
END_TEST

START_TEST(s21_is_equal_2) {
    s21_decimal value_1 = {{123453u, 654u, 0xFFFFFFFF, 80000000}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 80000000}};
    int return_value = s21_is_equal(value_1, value_2);
    ck_assert_int_eq(return_value, FALSE);
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");



    // Заменять "function" на название функции
    TCase *tc_is_equal_1;
    tc_is_equal_1 = tcase_create("s21_is_equal_1");
    suite_add_tcase(s, tc_is_equal_1);
    tcase_add_test(tc_is_equal_1, s21_is_equal_1);

    TCase *tc_is_equal_2;
    tc_is_equal_2 = tcase_create("s21_is_equal_2");
    suite_add_tcase(s, tc_is_equal_2);
    tcase_add_test(tc_is_equal_2, s21_is_equal_2);



    return s;
}

int main(void) {
    Suite *s;
    SRunner *sr;

    s = lib_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    srunner_free(sr);
    return 0;
}
