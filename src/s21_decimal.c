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

// s21_decimal value <<= n
void shift(s21_decimal* value, int n) {
    for (int i = 0; i < n; i++) {
        int bit31 = get_bit(*value, 31);
        int bit63 = get_bit(*value, 63);
        value->bits[0] <<= 1;
        value->bits[1] <<= 1;
        value->bits[2] <<= 1;
        if (bit31) value->bits[1] |= 1;
        if (bit63) value->bits[2] |= 1;
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

// потом можно убрать лишние buffer =
int bit_addition(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal* result) {
    s21_decimal res = {0, 0, 0, 0};
    int buffer = 0;

    for (int i = 0; i < 96; i++) {
        int current_bit_of_value_1 = get_bit(value_1, i);
        int current_bit_of_value_2 = get_bit(value_2, i);

        // оба бита нули
        if (current_bit_of_value_1 == 0 && current_bit_of_value_2 == 0) {
            if (buffer) {
                set_bit(&res, i, 1);
                buffer = 0;
            } else {
                set_bit(&res, i, 0);
            }
        } else if (current_bit_of_value_1 != current_bit_of_value_2) {
            if (buffer) {
                set_bit(&res, i, 0);
                buffer = 1;
            } else {
                set_bit(&res, i, 1);
            }
        } else {  // оба бита единицы
            if (buffer) {
                set_bit(&res, i, 1);
                buffer = 1;
            } else {
                set_bit(&res, i, 0);
                buffer = 1;
            }
        }

        int return_value = OK;
        if (i == 95 && buffer) return_value = INF;
        return return_value;
    }
}

int errors_s21mul(s21_decimal a, s21_decimal b) {
    int return_value = OK;
    if (last_bit(a) + last_bit(b) > 95 && !scale(a) && !scale(b))
        return_value = 1 + (sign_minus(a) != sign_minus(b));
    return return_value;
}

int are_all_bits_zero(s21_decimal value) {
    return (last_bit(value) == -1);
}

// int s21_mul(s21_decimal a, s21_decimal b, s21_decimal* result) {
//     result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;

//     int return_value = errors_s21mul(a, b);
//     if (return_value || !(are_all_bits_zero(a) && are_all_bits_zero(b))) {
//         for (int i = 0; i <= last_bit(b); i++) {
//             if (get_bit(b, i)) result = plus(a, result);
//             shift(&a, 1);
//         }
//         result->bits[3] = (a.bits[3] & 0x7FFFFFFF) + (b.bits[3] & 0x7FFFFFFF);
//         if (scale(result) > 28)
//             result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;
//         else if (sign_minus(a) != sign_minus(b))
//             result->bits[3] |= SIGNMASK;
        
//     }
//     return return_value;
// }

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int sign_result;
    if (get_sign(value_1) == get_sign(value_2))
        sign_result = 0;
    else
        sign_result = 1;

    // s21_decimal tmp = value_1;
    // for (int i = 0; i < 96; i++) {
    //     if (get_bit(value_2, i)) {
    //         s21_add(*result, tmp, result);
    //         shift(&tmp);
    //     }
    // }

    int return_value = OK;
    // само умножение
    s21_decimal tmp_res = {0, 0, 0, 0};
    int last_bit_1 = get_last_bit(value_1);
    for (int i = 0; i <= last_bit_1; i++) {
        if (get_bit(value_1, i)) {
            tmp_res = value_2;
            shift(&tmp_res, i);
            if (bit_addition(*result, tmp_res, result) == 1) {
                return_value = sign_result ? NEGATIVE_INF : INF;
            }
        }
    }

    if
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
