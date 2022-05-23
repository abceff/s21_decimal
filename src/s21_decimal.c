#include "s21_decimal.h"

int get_mask(unsigned int index, unsigned int n) {
    int res = 0;
    res = ~(4294967295 << n) << index;

    return res;
}

int get_element_number(int index) {
    unsigned int element_number;
    if (index <= 31)  // 0 <= index <= 31
        element_number = 0;
    else if (index <= 63)  // 32 <= index <= 63
        element_number = 1;
    else  // 64 <= index <= 95
        element_number = 2;
    return element_number;
}

int get_bit(s21_decimal value, int index) {
    unsigned int element_number = get_element_number(index);
    s21_decimal res;
    res.bits[element_number] = value.bits[element_number] &
                            get_mask(index, 1);
    return res.bits[element_number] ? 1 : 0;
}

void set_bit(s21_decimal* value, int index, int set_value) {
    unsigned int element_number = get_element_number(index);
    if (set_value)
        value->bits[element_number] = value->bits[element_number] |
                            get_mask(index, 1);
    else
        value->bits[element_number] = value->bits[element_number] &
                            ~get_mask(index, 1);
}

void shift(s21_decimal* value) {
    int bit31 = get_bit(*value, 31);
    int bit63 = get_bit(*value, 63);
    value->bits[0] <<= 1;
    value->bits[1] <<= 1;
    value->bits[2] <<= 1;
    if (bit31) value->bits[1] |= 1;
    if (bit63) value->bits[2] |= 1;
}

int get_sign(s21_decimal value) {
    return !!(value.bits[3] & get_mask(31, 1));
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int sign_result;
    if (get_sign(value_1) == get_sign(value_2))
        sign_result = 0;
    else
        sign_result = 1;

    s21_decimal tmp = value_1;
    for (int i = 0; i < 96; i++) {
        if (get_bit(value_2, i)) {
            s21_add(*result, tmp, result);
            shift(&tmp);
        }
    }
}

// int main() {
//     s21_decimal value;
//     for (int i = 0; i < 4; i++) {
//         value.bits[i] = 0;
//     }
//     value.bits[0] = 3;
//     set_bit(&value, 0, 0);
//     printf("%d\n", value.bits[0]);
//     return 0;
// }
