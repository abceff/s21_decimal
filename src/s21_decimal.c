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
    res.bits[element_number] = value.bits[element_number] & get_mask(index, 1);
    return res.bits[element_number] ? 1 : 0;
}

void set_bit(s21_decimal* value, int index, int set_value) {
    unsigned int element_number = get_element_number(index);
    if (set_value)
        value->bits[element_number] =
            value->bits[element_number] | get_mask(index, 1);
    else
        value->bits[element_number] =
            value->bits[element_number] & ~get_mask(index, 1);
}

void shift(s21_decimal* value, int n) {
    if (n > 0) {  // s21_decimal value <<= n
        for (int i = 0; i < n; i++) {
            int bit31 = get_bit(*value, 31);
            int bit63 = get_bit(*value, 63);
            value->bits[0] <<= 1;
            value->bits[1] <<= 1;
            value->bits[2] <<= 1;
            set_bit(value, 32, bit31);
            set_bit(value, 64, bit63);
        }
    } else {  // s21_decimal value >>= n
        for (int i = 0; i > n; i--) {
            int bit32 = get_bit(*value, 32);
            int bit64 = get_bit(*value, 64);
            value->bits[1] >>= 1;
            value->bits[0] >>= 1;
            value->bits[2] >>= 1;
            set_bit(value, 31, bit32);
            set_bit(value, 63, bit64);
        }
    }
}

int get_sign(s21_decimal value) { return !!(value.bits[3] & get_mask(31, 1)); }

int get_last_bit(s21_decimal value) {
    int last_bit = -1;
    for (int i = 0; i >= 0; i--) {
        if (get_bit(value, i)) {
            last_bit = i;
            break;
        }
    }
    return last_bit;
}

int are_all_bits_zero(s21_decimal value) { return (get_last_bit(value) == -1); }

void set_to_zero_all_bits(s21_decimal* value) {
    for (int i = 0; i < 4; i++) value->bits[i] = 0;
}

int bit_addition(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal* result) {
    int return_value = OK;
    int buffer = 0;

    for (int i = 0; i < 96; i++) {
        int current_bit_of_value_1 = get_bit(value_1, i);
        int current_bit_of_value_2 = get_bit(value_2, i);

        if (!current_bit_of_value_1 && !current_bit_of_value_2) {  // оба бита выключены
            if (buffer) {
                set_bit(result, i, 1);
                buffer = 0;
            } else {
                set_bit(result, i, 0);
            }
        } else if (current_bit_of_value_1 != current_bit_of_value_2) {
            if (buffer) {
                set_bit(result, i, 0);
                buffer = 1;
            } else {
                set_bit(result, i, 1);
            }
        } else {  // оба бита включены
            if (buffer) {
                set_bit(result, i, 1);
                buffer = 1;
            } else {
                set_bit(result, i, 0);
                buffer = 1;
            }
        }

        if (i == 95 && buffer == 1)
            return_value = INF;
    }
    return return_value;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = OK;
    set_to_zero_all_bits(result);
    if (!(are_all_bits_zero(value_1) || are_all_bits_zero(value_2))) {
        while (!are_all_bits_zero(value_2)) {
            if (get_bit(value_2, 0)) {
                bit_addition(*result, value_1, result);
            }
            shift(&value_2, -1);
        }
    }

    return return_value;
}

int main() {
    s21_decimal value_1;
    s21_decimal value_2;
    s21_decimal value_3;
    for (int i = 0; i < 4; i++) {
        value_1.bits[i] = 0;
        value_2.bits[i] = 0;
        value_3.bits[i] = 0;
    }
    value_1.bits[0] = 3;
    value_2.bits[0] = 2;
    s21_mul(value_1, value_2, &value_3);
    printf("%d\n", value_3.bits[0]);
    return 0;
}
