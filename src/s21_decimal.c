#include "s21_decimal.h"

/**
 * @brief Возвращает значение указанного бита
 * @param decVar число децимал
 * @param bit номер бита
 * @return значение указанного бита (0 - если бит равен нулю, 1 - bit on)
 */
int get_bit(const s21_decimal decVar, int bit) {
    int res = 0;
    if (bit / 32 < 4) {
        unsigned int mask = 1u << (bit % 32);
        res = decVar.bits[bit / 32] & mask;
    }
    return !!res;
}

/**
 * @brief Установить указанный бит вкл/выкл
 * @param varPtr указатель на число децимал
 * @param bit номер бита
 * @param value значение бита
 */
void set_bit(s21_decimal *varPtr, int bit, int value) {
    unsigned int mask = 1u << (bit % 32);
    if (bit / 32 < 4 && value) {
        varPtr->bits[bit / 32] |= mask;
    } else if (bit / 32 < 4 && !value) {
        varPtr->bits[bit / 32] &= ~mask;
    }
}

int last_bit(s21_decimal number) {
    int last_bit = 95;
    for (; last_bit >= 0 && get_bit(number, last_bit) == 0; last_bit--) {
    }
    return last_bit;
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

void offset_right(s21_decimal *varPtr, int value_offset) {
    for (int i = 0; i < value_offset; i++) {
        int value_32bit = get_bit(*varPtr, 32);
        int value_64bit = get_bit(*varPtr, 64);
        varPtr->bits[0] >>= 1;
        varPtr->bits[1] >>= 1;
        varPtr->bits[2] >>= 1;
        if (value_32bit) set_bit(varPtr, 31, 1);
        if (value_64bit) set_bit(varPtr, 63, 1);
    }
}

/**
 * @brief Установить знак числа децимал
 * @param varPtr указатель на число децимал
 * @param sign знак (0 - число положительное, 1 - отрецательное)
 */
void set_sign(s21_decimal *varPtr, int sign) {
    unsigned int mask = 1u << 31;
    if (sign != 0) {
        varPtr->bits[3] |= mask;
    } else {
        varPtr->bits[3] &= ~mask;
    }
}

/**
 * @brief Проверка знака числа децимал
 * @param varPtr указатель на число децимал
 * @return значение знака числа (0 - число положительное, 1 - отрецательное)
 */
int get_sign(const s21_decimal *varPtr) {
    unsigned int mask = 1u << 31;
    return !!(varPtr->bits[3] & mask);
}

/**
 * @brief Проверка степени числа децимал
 * @param varPtr указатель на число децимал
 * @return значение степени числа
 */
int get_scale(const s21_decimal *varPtr) {
    return (char)(varPtr->bits[3] >> 16);
}

/**
 * @brief Установить нужную степень числа децимал
 * @param varPtr указатель на число децимал
 * @param scale значение степени числа
 */
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

/**
 * @brief Являются ли числа нулями
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - являются (только когда оба),
 * 1 - не являются (по крайней мере одно)
 */
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

int bit_addition(s21_decimal var1, s21_decimal var2, s21_decimal *result) {
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

void mul_only_bits(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *result) {
    clear_bits(result);
    s21_decimal tmp_res;
    int last_bit_1 = last_bit(value_1);
    for (int i = 0; i <= last_bit_1; i++) {
        clear_bits(&tmp_res);
        int value_bit_1 = get_bit(value_1, i);

        if (value_bit_1) {
            tmp_res = value_2;
            offset_left(&tmp_res, i);
            bit_addition(*result, tmp_res, result);
        }
    }
}

void scale_equalize(s21_decimal *value_1, s21_decimal* value_2) {
    s21_decimal ten = {{10u, 0, 0, 0}};
    if (get_scale(value_1) < get_scale(value_2)) {
        int difference = get_scale(value_2) - get_scale(value_1);
        if (get_bit(*value_1, 93) == 0 && get_bit(*value_1, 94) == 0 &&
                get_bit(*value_1, 95) == 0) {
            for (int i = 0; i < difference; i++) {
                mul_only_bits(*value_1, ten, value_1);
            }
            set_scale(value_1, get_scale(value_2));
        } else {
            offset_right(value_2, difference);
            set_scale(value_2, get_scale(value_1));
        }
    } else {
        int difference = get_scale(value_1) - get_scale(value_2);
        if (get_bit(*value_2, 93) == 0 && get_bit(*value_2, 94) == 0 &&
                get_bit(*value_2, 95) == 0) {
            for (int i = 0; i < difference; i++) {
                mul_only_bits(*value_2, ten, value_2);
            }
            set_scale(value_2, get_scale(value_2));
        } else {
            offset_right(value_1, difference);
            set_scale(value_1, get_scale(value_2));
        }
    }
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
    int res;
    if (zero_check(value_1, value_2)) {
        res = FALSE;
    } else if (get_sign(&value_1) > get_sign(&value_2)) {
        res = FALSE;
    } else if (get_sign(&value_1) < get_sign(&value_2)) {
        res = TRUE;
    } else {
        if (get_scale(&value_1) != get_scale(&value_2)) {
            scale_equalize(&value_1, &value_2);
        }
        if (value_1.bits[2] > value_2.bits[2]) {
            res = TRUE;
        } else if (value_1.bits[2] < value_2.bits[2]) {
            res = FALSE;
        } else {
            if (value_1.bits[1] > value_2.bits[1]) {
                res = TRUE;
            } else if (value_1.bits[1] < value_2.bits[1]) {
                res = FALSE;
            } else {
                if (value_1.bits[0] > value_2.bits[0]) {
                    res = TRUE;
                } else {
                    res = FALSE;
                }
            }
        }
    }

    if (get_sign(&value_1) && get_sign(&value_2)) res = !res;

    return res;
}

void clear_bits(s21_decimal *varPtr) {
    memset(varPtr->bits, 0, sizeof(varPtr->bits));
}

// int main() {
//     s21_decimal value_1 = {{1234, 0, 0, 0}};
//     s21_decimal value_2 = {{2, 0, 0, 0}};
//     s21_decimal ten = {{10, 0, 0, 0}};
//     s21_decimal result = {{0, 0, 0, 0}};
//     set_scale(&value_1, 1);
//     // s21_decimal result = {{0, 0, 0, 0}};
//     int is_greater = s21_is_greater(value_1, value_2);
//     mul_only_bits(value_1, ten, &result);
//     printf("is_greater == %d\n", is_greater);
//     printf("result.bits[0] == %u\n", result.bits[0]);
//     printf("result.bits[1] == %u\n", result.bits[1]);
//     printf("result.bits[2] == %u\n", result.bits[2]);
//     printf("result.bits[3] == %u\n", result.bits[3]);
// }