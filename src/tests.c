#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_add_1) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 1);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 2);
    s21_decimal result;
    s21_decimal check = {{416, 0, 0, 0}};
    set_scale(&check, 2);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_2) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal value_2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    set_sign(&value_2, 1);
    s21_decimal result = {{0, 0, 0, 0}};
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_3) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
    set_scale(&value_1, 1);
    s21_decimal value_2 = {{2, 0, 0, 0}};
    set_scale(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{0x1, 0x0, 0x1, 0}};
    set_scale(&check, 1);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_4) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 2);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{2873, 0, 0, 0}};
    set_scale(&check, 2);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_5) {
    s21_decimal value_1 = {{1, 0, 0, 0}};
    s21_decimal value_2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    set_scale(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{0x9999999a, 0x99999999, 0x19999999, 0}};
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_6) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    set_scale(&value_1, 1);
    s21_decimal value_2 = {{1, 0, 0, 0}};
    s21_decimal result;
    s21_decimal check = {{0x9999999a, 0x99999999, 0x19999999, 0}};
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_7) {
    s21_decimal value_1 = {{1, 0, 0, 0}};
    s21_decimal value_2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal result;
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, INF);
}
END_TEST

START_TEST(s21_add_8) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 2);
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{2847, 0, 0, 0}};
    set_scale(&check, 2);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_9) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 1);
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 2);
    set_sign(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{416, 0, 0, 0}};
    set_scale(&check, 2);
    set_sign(&check, 1);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_add_10) {
    s21_decimal value_1 = {{1614, 0, 0, 0}};
    set_scale(&value_1, 3);
    s21_decimal value_2 = {{46071, 0, 0, 0}};
    set_scale(&value_2, 2);
    s21_decimal result;
    s21_decimal check = {{462324, 0, 0, 0}};
    set_scale(&check, 3);
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_sub_1) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 1);
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 2);
    set_sign(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{156, 0, 0, 0}};
    set_scale(&check, 2);
    int return_value = s21_sub(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_sub_2) {
    s21_decimal value_1 = {{13, 0, 0, 0}};
    set_scale(&value_1, 1);
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{286, 0, 0, 0}};
    set_scale(&value_2, 2);
    s21_decimal result;
    s21_decimal check = {{416, 0, 0, 0}};
    set_scale(&check, 2);
    set_sign(&check, 1);
    int return_value = s21_sub(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_sub_3) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{2, 0, 0, 0}};
    s21_decimal result;
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_sub(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, NEGATIVE_INF);
}
END_TEST

START_TEST(s21_sub_4) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    set_sign(&value_1, 1);
    s21_decimal value_2 = {{2, 0, 0, 0}};
    set_sign(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{0, 0, 0, 0}};
    int return_value = s21_add(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, NEGATIVE_INF);
}
END_TEST

START_TEST(s21_sub_5) {
    s21_decimal value_1 = {{64071, 0, 0, 0}};
    set_scale(&value_1, 4);
    s21_decimal value_2 = {{5919, 0, 0, 0}};
    set_scale(&value_2, 1);
    s21_decimal result;
    s21_decimal check = {{5854929, 0, 0, 0}};
    set_scale(&check, 4);
    set_sign(&check, 1);
    int return_value = s21_sub(value_1, value_2, &result);
    ck_assert_int_eq(s21_is_equal(result, check), 1);
    ck_assert_int_eq(return_value, OK);
}
END_TEST



Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");

    // Заменять "function" на название функции
    TCase *tc_add_1;
    tc_add_1 = tcase_create("s21_add_1");
    suite_add_tcase(s, tc_add_1);
    tcase_add_test(tc_add_1, s21_add_1);

    TCase *tc_add_2;
    tc_add_2 = tcase_create("s21_add_2");
    suite_add_tcase(s, tc_add_2);
    tcase_add_test(tc_add_2, s21_add_2);

    TCase *tc_add_3;
    tc_add_3 = tcase_create("s21_add_3");
    suite_add_tcase(s, tc_add_3);
    tcase_add_test(tc_add_3, s21_add_3);

    TCase *tc_add_4;
    tc_add_4 = tcase_create("s21_add_4");
    suite_add_tcase(s, tc_add_4);
    tcase_add_test(tc_add_4, s21_add_4);

    TCase *tc_add_5;
    tc_add_5 = tcase_create("s21_add_5");
    suite_add_tcase(s, tc_add_5);
    tcase_add_test(tc_add_5, s21_add_5);

    TCase *tc_add_6;
    tc_add_6 = tcase_create("s21_add_6");
    suite_add_tcase(s, tc_add_6);
    tcase_add_test(tc_add_6, s21_add_6);

    TCase *tc_add_7;
    tc_add_7 = tcase_create("s21_add_7");
    suite_add_tcase(s, tc_add_7);
    tcase_add_test(tc_add_7, s21_add_7);

    TCase *tc_add_8;
    tc_add_8 = tcase_create("s21_add_8");
    suite_add_tcase(s, tc_add_8);
    tcase_add_test(tc_add_8, s21_add_8);

    TCase *tc_add_9;
    tc_add_9 = tcase_create("s21_add_9");
    suite_add_tcase(s, tc_add_9);
    tcase_add_test(tc_add_9, s21_add_9);

    TCase *tc_add_10;
    tc_add_10 = tcase_create("s21_add_10");
    suite_add_tcase(s, tc_add_10);
    tcase_add_test(tc_add_10, s21_add_10);

    TCase *tc_sub_1;
    tc_sub_1 = tcase_create("s21_sub_1");
    suite_add_tcase(s, tc_sub_1);
    tcase_add_test(tc_sub_1, s21_sub_1);

    TCase *tc_sub_2;
    tc_sub_2 = tcase_create("s21_sub_2");
    suite_add_tcase(s, tc_sub_2);
    tcase_add_test(tc_sub_2, s21_sub_2);

    TCase *tc_sub_3;
    tc_sub_3 = tcase_create("s21_sub_3");
    suite_add_tcase(s, tc_sub_3);
    tcase_add_test(tc_sub_3, s21_sub_3);

    TCase *tc_sub_4;
    tc_sub_4 = tcase_create("s21_sub_4");
    suite_add_tcase(s, tc_sub_4);
    tcase_add_test(tc_sub_4, s21_sub_4);

    TCase *tc_sub_5;
    tc_sub_5 = tcase_create("s21_sub_5");
    suite_add_tcase(s, tc_sub_5);
    tcase_add_test(tc_sub_5, s21_sub_5);

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
