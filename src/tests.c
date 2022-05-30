#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

s21_decimal case_max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
s21_decimal case_min = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
s21_decimal case1 = {{0, 0, 0, 0}};
s21_decimal case2 = {{1, 0, 0, 0}};
s21_decimal case3 = {{0x7FFFFFFF, 0, 0, 0}};
s21_decimal case4 = {{0x7FFFFFFF, 0, 0, 0x80000000}};
s21_decimal case5 = {{0x80000000, 0, 0, 0x90000}};
s21_decimal case6 = {{755, 0, 0, 0x10000}};
s21_decimal case7 = {{75, 0, 0, 0}};
s21_decimal case8 = {{7551, 0, 0, 0x80020000}};
s21_decimal case9 = {{76, 0, 0, 0x80000000}};
s21_decimal case10 = {{755, 0, 0, 0}};
s21_decimal case11 = {{5, 0, 0, 0}};
s21_decimal case12 = {{150, 0, 0, 0x80000000}};
s21_decimal case13 = {{2, 0, 0, 0x80000000}};

START_TEST(s21_mul_1) {
    s21_decimal value_1 = {{5, 0, 0, 0}};
    s21_decimal value_2 = {{7, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{35, 0, 0, 0}};
    int return_value = s21_mul(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_mul_2) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0, 0, 0}};
    s21_decimal value_2 = {{0xFFFFFFFF, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{1, 0xFFFFFFFE, 0, 0}};
    int return_value = s21_mul(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_mul_3) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_mul(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, INF);
}
END_TEST

START_TEST(s21_mul_4) {
    s21_decimal value_1 = {{123456u, 123u, 0, 0}};
    s21_decimal value_2 = {{654321u, 654u, 0, 0}};
    set_sign(&value_2, 1);
    set_scale(&value_1, 2);
    set_scale(&value_2, 3);
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{0xcedabe40, 0x99c0c5d, 0x13a3a, 0x80050000}};
    int return_value = s21_mul(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");

    // Заменять "function" на название функции
    TCase *tc_mul_1;
    tc_mul_1 = tcase_create("s21_mul_1");
    suite_add_tcase(s, tc_mul_1);
    tcase_add_test(tc_mul_1, s21_mul_1);

    TCase *tc_mul_2;
    tc_mul_2 = tcase_create("s21_mul_2");
    suite_add_tcase(s, tc_mul_2);
    tcase_add_test(tc_mul_1, s21_mul_2);

    TCase *tc_mul_3;
    tc_mul_3 = tcase_create("s21_mul_3");
    suite_add_tcase(s, tc_mul_3);
    tcase_add_test(tc_mul_1, s21_mul_3);

    TCase *tc_mul_4;
    tc_mul_4 = tcase_create("s21_mul_4");
    suite_add_tcase(s, tc_mul_4);
    tcase_add_test(tc_mul_1, s21_mul_4);

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