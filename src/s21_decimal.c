#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
    int is_equal = TRUE;

    if (value_1.bits[0] != value_2.bits[0] ||
        value_1.bits[1] != value_2.bits[1] ||
        value_1.bits[2] != value_2.bits[2] ||
        value_1.bits[3] != value_2.bits[3])
        is_equal = FALSE;

    return is_equal;
}

int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2) {
    return (!s21_is_equal(dec1, dec2));
}