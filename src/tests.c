#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_truncate_1) {
    s21_decimal value_1 = {{35, 0, 0, 0}};
    s21_decimal check = {{3, 0, 0, 0}};
    set_scale(&value_1, 1);
    s21_truncate(value_1, &value_1);
    ck_assert_int_eq(s21_is_equal(value_1, check), 1);
}
END_TEST

START_TEST(s21_truncate_2) {
    s21_decimal value_1 = {{123456, 0, 0, 0}};
    set_scale(&value_1, 3);
    set_sign(&value_1, 1);
    s21_decimal check = {{123, 0, 0, 0}};
    set_sign(&check, 1);
    s21_truncate(value_1, &value_1);
    ck_assert_int_eq(s21_is_equal(value_1, check), 1);
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");

    // Заменять "function" на название функции
    TCase *tc_truncate_1;
    tc_truncate_1 = tcase_create("s21_truncate_1");
    suite_add_tcase(s, tc_truncate_1);
    tcase_add_test(tc_truncate_1, s21_truncate_1);

    TCase *tc_truncate_2;
    tc_truncate_2 = tcase_create("s21_truncate_2");
    suite_add_tcase(s, tc_truncate_2);
    tcase_add_test(tc_truncate_2, s21_truncate_2);

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