#include "s21_decimal.h"

int zero_check(s21_decimal dec1, s21_decimal dec2) {
  int is_zero = FALSE;
  s21_decimal *pt1 = &dec1;
  s21_decimal *pt2 = &dec2;

  if (pt1 && pt2) {
    if (!dec1.bits[0] && !dec2.bits[0] && !dec1.bits[1] && !dec2.bits[1] &&
        !dec1.bits[2] && !dec2.bits[2])
      is_zero = TRUE;
  }
  return is_zero;
}

int get_bit(const s21_decimal decVar, int bit) {
  int res = 0;
  if (bit / 32 < 4) {
    unsigned int mask = 1u << (bit % 32);
    res = decVar.bits[bit / 32] & mask;
  }
  return !!res;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
    int is_equal = -1;

    if (is_equal == -1) {
        if (zero_check(value_1, value_2) == TRUE) is_equal = TRUE;
    }

    for (int i = 95; i >= 0 && is_equal == -1; i--) {
        int bit_dec1 = get_bit(value_1, i);
        int bit_value_2 = get_bit(value_2, i);
        if (bit_dec1 != bit_value_2) is_equal = FALSE;
    }

    return (is_equal != FALSE) ? TRUE : FALSE;
}

int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2) {
    return (!s21_is_equal(dec1, dec2));
}