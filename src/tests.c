#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_div_1) {
    s21_decimal value_1 = {{35, 0, 0, 0}};
    s21_decimal value_2 = {{5, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{70, 0, 0, 0}};
    set_scale(&value_1, 1);
    set_scale(&value_2, 2);
    int return_value = s21_div(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_div_2) {
    s21_decimal value_1 = {{0x88888888, 0x88888888, 0x88888888, 0}};
    s21_decimal value_2 = {{0x2, 0, 0, 0}};
    set_sign(&value_2, 1);
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{0x44444444, 0x44444444, 0x44444444, 0}};
    set_sign(&check, 1);
    int return_value = s21_div(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_div_3) {
    s21_decimal value_1 = {{10, 0, 0, 0}};
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{8, 0, 0, 0}};
    set_sign(&value_2, 1);
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{125, 0, 0, 0}};
    set_scale(&check, 2);
    int return_value = s21_div(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_div_4) {
    s21_decimal value_1 = {{15, 0, 0, 0}};
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{0, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_div(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, DIVISION_BY_ZERO);
}
END_TEST

START_TEST(s21_div_5) {
    s21_decimal value_1 = {{10, 0, 0, 0}};
    s21_decimal value_2 = {{1, 0, 0, 0}};
    set_scale(&value_2, 2);
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{1000u, 0, 0, 0}};
    int return_value = s21_div(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");

    // Заменять "function" на название функции
    TCase *tc_div_1;
    tc_div_1 = tcase_create("s21_div_1");
    suite_add_tcase(s, tc_div_1);
    tcase_add_test(tc_div_1, s21_div_1);

    TCase *tc_div_2;
    tc_div_2 = tcase_create("s21_div_2");
    suite_add_tcase(s, tc_div_2);
    tcase_add_test(tc_div_2, s21_div_2);

    TCase *tc_div_3;
    tc_div_3 = tcase_create("s21_div_3");
    suite_add_tcase(s, tc_div_3);
    tcase_add_test(tc_div_3, s21_div_3);

    TCase *tc_div_4;
    tc_div_4 = tcase_create("s21_div_4");
    suite_add_tcase(s, tc_div_4);
    tcase_add_test(tc_div_4, s21_div_4);

    TCase *tc_div_5;
    tc_div_5 = tcase_create("s21_div_5");
    suite_add_tcase(s, tc_div_5);
    tcase_add_test(tc_div_5, s21_div_5);

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