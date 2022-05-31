#include "s21_decimal.h"

int get_bit(const s21_decimal decVar, int bit) {
    int res = 0;
    if (bit / 32 < 4) {
        unsigned int mask = 1u << (bit % 32);
        res = decVar.bits[bit / 32] & mask;
    }
    return !!res;
}

void set_bit(s21_decimal *varPtr, int bit, int value) {
    unsigned int mask = 1u << (bit % 32);
    if (bit / 32 < 4 && value) {
        varPtr->bits[bit / 32] |= mask;
    } else if (bit / 32 < 4 && !value) {
        varPtr->bits[bit / 32] &= ~mask;
    }
}

void set_sign(s21_decimal *varPtr, int sign) {
    unsigned int mask = 1u << 31;
    if (sign != 0) {
        varPtr->bits[3] |= mask;
    } else {
        varPtr->bits[3] &= ~mask;
    }
}

int get_sign(const s21_decimal *varPtr) {
    unsigned int mask = 1u << 31;
    return !!(varPtr->bits[3] & mask);
}

int get_scale(const s21_decimal *varPtr) {
    return (char)(varPtr->bits[3] >> 16);
}

void set_scale(s21_decimal *varPtr, int scale) {
    if (scale < 0 || scale > 28) {
        printf("wrong scale = %d\n", scale);
    } else {
        int clearMask = ~(0xFF << 16);
        varPtr->bits[3] &= clearMask;
        int mask = scale << 16;
        varPtr->bits[3] |= mask;
    }
}

int offset_left(s21_decimal *varPtr, int value_offset) {
    int return_value = OK;
    int lastbit = last_bit(*varPtr);
    if (lastbit + value_offset > 95) {
        return_value = INF;
    } else {
        for (int i = 0; i < value_offset; i++) {
            int value_31bit = get_bit(*varPtr, 31);
            int value_63bit = get_bit(*varPtr, 63);
            varPtr->bits[0] <<= 1;
            varPtr->bits[1] <<= 1;
            varPtr->bits[2] <<= 1;
            if (value_31bit) set_bit(varPtr, 32, 1);
            if (value_63bit) set_bit(varPtr, 64, 1);
        }
    }
    return return_value;
}

int last_bit(s21_decimal number) {
    int last_bit = 95;
    for (; last_bit >= 0 && get_bit(number, last_bit) == 0; last_bit--) {
    }
    return last_bit;
}

int bit_addition(s21_decimal var1, s21_decimal var2, s21_decimal* result) {
    int return_value = OK;
    // s21_decimal res = {0, 0, 0, 0};
    int buffer = 0;

    for (int i = 0; i < 96; i++) {
        int cur_bit_of_var1 = get_bit(var1, i);
        int cur_bit_of_var2 = get_bit(var2, i);

        if (!cur_bit_of_var1 && !cur_bit_of_var2) {  // оба бита выключены
            if (buffer) {
                set_bit(result, i, 1);
                buffer = 0;
            } else {
                set_bit(result, i, 0);
            }
        } else if (cur_bit_of_var1 != cur_bit_of_var2) {
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
            return_value = INF;  // переполнение нужно вывести инфинити
    }

    return return_value;
}

int s21_mul(s21_decimal number_1, s21_decimal number_2, s21_decimal* result) {
    int return_value = OK;
    int sign_result;

    if (get_sign(&number_1) != get_sign(&number_2)) {
        sign_result = 1;
    } else {
        sign_result = 0;
    }

    int last_bit_1 = last_bit(number_1);  // Находим последнюю единицу справа налево
    s21_decimal tmp_res = {{0, 0, 0, 0}};

    int bit_addition_result = OK;
    // само умножение
    for (int i = 0; i <= last_bit_1; i++) {

        clear_bits(&tmp_res);
        int value_bit_1 = get_bit(number_1, i);

        if (value_bit_1) {
            tmp_res = number_2;
            bit_addition_result = offset_left(&tmp_res, i);
            bit_addition_result = bit_addition(*result, tmp_res, result);
        }
    }

    if (bit_addition_result == INF) {
        if (sign_result)
            return_value = NEGATIVE_INF;
        else
            return_value = INF;
        clear_bits(result);
    } else {
        int scale = get_scale(&number_1) + get_scale(&number_2);
        set_scale(result, scale);
        set_sign(result, sign_result);
    }

    return return_value;
}

void clear_bits(s21_decimal *varPtr) {
    memset(varPtr->bits, 0, sizeof(varPtr->bits));
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
//     s21_decimal value_1;
//     s21_decimal value_2;
//     s21_decimal value_3;
//     for (int i = 0; i < 4; i++) {
//         value_1.bits[i] = 0;
//         value_2.bits[i] = 0;
//         value_3.bits[i] = 0;
//     }
//     value_1.bits[0] = 0xFFFFFFFF;
//     value_1.bits[1] = 0xFFFFFFFF;
//     value_1.bits[2] = 0xFFFFFFFF;
//     value_2.bits[0] = 3u;
//     set_scale(&value_2, 5);
//     int return_value = s21_mul(value_1, value_2, &value_3);
//     printf("return_value == %d\n", return_value);
//     printf("value_3.bits[0] == %x\n", value_3.bits[0]);
//     printf("value_3.bits[1] == %x\n", value_3.bits[1]);
//     printf("value_3.bits[2] == %x\n", value_3.bits[2]);
//     printf("value_3.bits[3] == %x\n", value_3.bits[3]);

//     s21_decimal check = {{0xcedabe40, 0x99c0c5d, 0x13a3a, 0x80050000}};
//     printf("s21_is_equal == %d\n", s21_is_equal(value_3, check));
//     for (int i = 95; i >= 0; i--) {
//         printf("%d", get_bit(value_3, i));
//     }
//     printf("\n");
//     return 0;
// }