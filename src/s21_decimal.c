#include "s21_decimal.h"

void s21_set_sign(s21_decimal *varPtr, int sign) {
  unsigned int mask = 1u << 31;
  if (sign != 0) {
    varPtr->bits[3] |= mask;
  } else {
    varPtr->bits[3] &= ~mask;
  }
}

int s21_get_sign(const s21_decimal *varPtr) {
  unsigned int mask = 1u << 31;
  return !!(varPtr->bits[3] & mask);
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  int sign = s21_get_sign(&value);
  if (sign) {
    s21_set_sign(result, 0);
  } else {
    s21_set_sign(result, 1);
  }
  return 0;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int is_equal = TRUE;

  if (value_1.bits[0] == 0 && value_2.bits[0] == 0 && value_1.bits[1] == 0 &&
      value_2.bits[1] == 0 && value_1.bits[2] == 0 && value_2.bits[2] == 0) {
    // is_equal = True
  } else if (value_1.bits[0] != value_2.bits[0] ||
             value_1.bits[1] != value_2.bits[1] ||
             value_1.bits[2] != value_2.bits[2] ||
             value_1.bits[3] != value_2.bits[3])
    is_equal = FALSE;

  return is_equal;
}

// int main() {
//   s21_decimal value_2 = {{0, 0, 0, 0}};
//   s21_decimal value_1 = {{0, 0, 0, 0}};
//   //   ;
//   //   // s21_decimal value_3;
//   //   //  s21_decimal remainder;
//   //   //   for (int i = 0; i < 4; i++) {
//   //   //     value_1.bits[i] = 0;
//   //   //     value_2.bits[i] = 0;
//   //   //     // value_3.bits[i] = 0;
//   //   //     //  remainder.bits[i] = 0;
//   //   //   }

//   // s21_set_sign(&value_1, 1);
//   s21_negate(value_1, &value_2);
//   // s21_negate(value_2, &value_2);
//   //  s21_set_sign(&value_1, 0);
//   printf("value_2.bits[0] == %u\n", value_2.bits[0]);
//   printf("value_2.bits[1] == %x\n", value_2.bits[1]);
//   printf("value_2.bits[2] == %x\n", value_2.bits[2]);
//   printf("value_2.bits[3] == %x\n", value_2.bits[3]);

//   printf("value_1.bits[0] == %u\n", value_1.bits[0]);
//   printf("value_1.bits[1] == %x\n", value_1.bits[1]);
//   printf("value_1.bits[2] == %x\n", value_1.bits[2]);
//   printf("value_1.bits[3] == %x\n", value_1.bits[3]);
//   return 0;
// }