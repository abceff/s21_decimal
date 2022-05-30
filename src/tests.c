#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_is_less_or_equal_1) {
    s21_decimal value_1 = {{123457u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, FALSE);
}
END_TEST

START_TEST(s21_is_less_or_equal_2) {
    s21_decimal value_1 = {{123457u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    set_sign(&value_1, 1);
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, TRUE);
}
END_TEST

START_TEST(s21_is_less_or_equal_3) {
    s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123457u, 654u, 0xFFFFFFFF, 0}};
    set_sign(&value_2, 1);
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, FALSE);
}
END_TEST

START_TEST(s21_is_less_or_equal_4) {
    s21_decimal value_1 = {{0, 0, 0, 0}};
    s21_decimal value_2 = {{0, 0, 0, 0}};
    set_sign(&value_2, 1);
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, TRUE);
}
END_TEST

START_TEST(s21_is_less_or_equal_5) {
    s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123457u, 654u, 0xFFFFFFFF, 0}};
    set_sign(&value_1, 1);
    set_sign(&value_2, 1);
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, FALSE);
}
END_TEST

START_TEST(s21_is_less_or_equal_6) {
    s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    set_scale(&value_2, 2);
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, FALSE);
}
END_TEST

START_TEST(s21_is_less_or_equal_7) {
    s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 0}};
    int return_value = s21_is_less_or_equal(value_1, value_2);
    ck_assert_int_eq(return_value, TRUE);
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");



    // Заменять "function" на название функции
    TCase *tc_is_less_or_equal_1;
    tc_is_less_or_equal_1 = tcase_create("s21_is_less_or_equal_1");
    suite_add_tcase(s, tc_is_less_or_equal_1);
    tcase_add_test(tc_is_less_or_equal_1, s21_is_less_or_equal_1);

    TCase *tc_is_less_or_equal_2;
    tc_is_less_or_equal_2 = tcase_create("s21_is_less_or_equal_2");
    suite_add_tcase(s, tc_is_less_or_equal_2);
    tcase_add_test(tc_is_less_or_equal_2, s21_is_less_or_equal_2);

    TCase *tc_is_less_or_equal_3;
    tc_is_less_or_equal_3 = tcase_create("s21_is_less_or_equal_3");
    suite_add_tcase(s, tc_is_less_or_equal_3);
    tcase_add_test(tc_is_less_or_equal_3, s21_is_less_or_equal_3);

    TCase *tc_is_less_or_equal_4;
    tc_is_less_or_equal_4 = tcase_create("s21_is_less_or_equal_4");
    suite_add_tcase(s, tc_is_less_or_equal_4);
    tcase_add_test(tc_is_less_or_equal_4, s21_is_less_or_equal_4);

    TCase *tc_is_less_or_equal_5;
    tc_is_less_or_equal_5 = tcase_create("s21_is_less_or_equal_5");
    suite_add_tcase(s, tc_is_less_or_equal_5);
    tcase_add_test(tc_is_less_or_equal_5, s21_is_less_or_equal_5);

    TCase *tc_is_less_or_equal_6;
    tc_is_less_or_equal_6 = tcase_create("s21_is_less_or_equal_6");
    suite_add_tcase(s, tc_is_less_or_equal_6);
    tcase_add_test(tc_is_less_or_equal_6, s21_is_less_or_equal_6);

    TCase *tc_is_less_or_equal_7;
    tc_is_less_or_equal_7 = tcase_create("s21_is_less_or_equal_7");
    suite_add_tcase(s, tc_is_less_or_equal_7);
    tcase_add_test(tc_is_less_or_equal_7, s21_is_less_or_equal_7);



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
