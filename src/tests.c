#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_negate_1) {
  s21_decimal value_1 = {{5, 0, 0, 0}};
  s21_decimal check = {{5, 0, 0, 0}};
  s21_decimal result = {{5, 0, 0, 0}};
  s21_set_sign(&value_1, 1);
  int return_value = s21_negate(value_1, &result);
  ck_assert_int_eq(s21_is_equal(result, check), 1);
  ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_negate_2) {
  s21_decimal value_1 = {{1, 1, 1, 0}};
  s21_decimal check = {{1, 1, 1, 0x80000000}};
  s21_decimal result = {{1, 1, 1, 0}};
  int return_value = s21_negate(value_1, &result);
  ck_assert_int_eq(s21_is_equal(result, check), 1);
  ck_assert_int_eq(return_value, OK);
}
END_TEST

START_TEST(s21_negate_3) {
  s21_decimal value_1 = {{1, 1, 1, 0}};
  s21_decimal check = {{1, 1, 1, 0x80000000}};
  s21_decimal result = {{1, 1, 1, 0}};
  s21_set_sign(&value_1, 1);
  s21_negate(value_1, &result);
  int return_value = s21_negate(result, &result);
  ck_assert_int_eq(s21_is_equal(result, check), 1);
  ck_assert_int_eq(return_value, OK);
}
END_TEST

Suite *lib_suite(void) {
  Suite *s;
  s = suite_create("Check");

  // Заменять "function" на название функции
  TCase *tc_negate_1;
  tc_negate_1 = tcase_create("s21_negate_1");
  suite_add_tcase(s, tc_negate_1);
  tcase_add_test(tc_negate_1, s21_negate_1);

  TCase *tc_negate_2;
  tc_negate_2 = tcase_create("s21_negate_2");
  suite_add_tcase(s, tc_negate_2);
  tcase_add_test(tc_negate_2, s21_negate_2);

  TCase *tc_negate_3;
  tc_negate_3 = tcase_create("s21_negate_3");
  suite_add_tcase(s, tc_negate_3);
  tcase_add_test(tc_negate_3, s21_negate_3);

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