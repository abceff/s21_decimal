#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

START_TEST(s21_function) {
    
}
END_TEST

Suite *lib_suite(void) {
    Suite *s;
    s = suite_create("Check");



    // Заменять "function" на название функции
    TCase *tc_function;
    tc_function = tcase_create("s21_function");
    suite_add_tcase(s, tc_function);
    tcase_add_test(tc_function, s21_function);



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
