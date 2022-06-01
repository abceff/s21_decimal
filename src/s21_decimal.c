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
    int clearMask = ~(0xFF << 16);
    varPtr->bits[3] &= clearMask;
    int mask = scale << 16;
    varPtr->bits[3] |= mask;
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

int s21_add(s21_decimal number_1, s21_decimal number_2, s21_decimal *result) {
    clear_bits(result);
    int return_value = OK;

    if (!get_sign(&number_1) && !get_sign(&number_2)) {
        //  оба числа положительных

        if (get_scale(&number_1) != get_scale(&number_2)) {
            scale_equalize(&number_1, &number_2);
        }

        int bit_additioin_result = OK;
        s21_decimal tmpRes;
        bit_additioin_result = bit_addition(number_1, number_2, &tmpRes);

        if (bit_additioin_result == INF) {
            return_value = INF;
        } else {
            // результат можно сразу присвоить
            *result = tmpRes;
            result->bits[3] = number_1.bits[3];
        }

    } else if (get_sign(&number_1) && !get_sign(&number_2)) {
        // 1 число отрецательное 2 число положительное
        // вызывается функция вычитания которая
        // создает доп код и сново вызывает сложение
        set_sign(&number_1, 0);
        return_value = s21_sub(number_2, number_1, result);

    } else if (!get_sign(&number_1) && get_sign(&number_2)) {
        // 1 полож 2 отрец
        set_sign(&number_2, 0);
        return_value = s21_sub(number_1, number_2, result);

    } else {
        // оба отрицательных
        set_sign(&number_1, 0);
        set_sign(&number_2, 0);
        return_value = s21_add(number_1, number_2, result);
        if (return_value == INF)
            return_value = NEGATIVE_INF;
        else
            set_sign(result, 1);
    }

    return return_value;
}

void sub_only_bits(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *result) {
    clear_bits(result);

    // знаки одинаковые - ситуация вырождается в -+ или +-
    if (s21_is_equal(value_1, value_2) == TRUE) {
        // они равны, все ок, ничего не делаем, в результате уже лежит 0

    } else {
        int value_1_last_bit = last_bit(value_1);
        int buffer = 0;
        for (int i = 0; i <= value_1_last_bit; i++) {
            int current_bit_of_value_1 = get_bit(value_1, i);
            int current_bit_of_value_2 = get_bit(value_2, i);

            if (!current_bit_of_value_1 && !current_bit_of_value_2) {
                if (buffer) {
                    buffer = 1;
                    set_bit(result, i, 1);
                } else {
                    set_bit(result, i, 0);
                }
            } else if (current_bit_of_value_1 && !current_bit_of_value_2) {
                if (buffer) {
                    buffer = 0;
                    set_bit(result, i, 0);
                } else {
                    set_bit(result, i, 1);
                }
            } else if (!current_bit_of_value_1 && current_bit_of_value_2) {
                if (buffer) {
                    buffer = 1;
                    set_bit(result, i, 0);
                } else {
                    buffer = 1;
                    set_bit(result, i, 1);
                }
            } else if (current_bit_of_value_1 && current_bit_of_value_2) {
                if (buffer) {
                    buffer = 1;
                    set_bit(result, i, 1);
                } else {
                    set_bit(result, i, 0);
                }
            }
        }
    }
}

int s21_sub(s21_decimal number_1, s21_decimal number_2, s21_decimal *result) {
    clear_bits(result);
    int return_value = OK;

    int sign_1 = get_sign(&number_1);
    int sign_2 = get_sign(&number_2);
    if (get_scale(&number_1) != get_scale(&number_2)) {
        scale_equalize(&number_1, &number_2);
    }
    set_sign(&number_1, sign_1);
    set_sign(&number_2, sign_2);

    int resultSign;

    // проверяем на знаки
    if (get_sign(&number_1) != get_sign(&number_2)) {
        // знаки разные - ситуация вырождается в ++ или --
        resultSign = get_sign(&number_1);
        set_sign(&number_1, 0);
        set_sign(&number_2, 0);
        return_value = s21_add(number_1, number_2, result);
        if (return_value == INF)
            return_value = NEGATIVE_INF;
        else
            set_sign(result, resultSign);

    } else {
        // знаки одинаковые - ситуация вырождается в -+ или +-
        if (s21_is_equal(number_1, number_2) == TRUE) {
            // они равны, все ок, ничего не делаем, в результате уже лежит 0

        } else {
            // числа разные, значит нужно знать какой знак проставлять и тд
            int sign1 = get_sign(&number_1);
            int sign2 = get_sign(&number_2);
            set_sign(&number_1, 0);
            set_sign(&number_2, 0);
            s21_decimal *smallPtr, *bigPtr;

            // кто из них больше по модулю
            if (s21_is_less(number_1, number_2) == TRUE) {
                smallPtr = &number_1;
                bigPtr = &number_2;
                resultSign = !sign2;
            } else {
                smallPtr = &number_2;
                bigPtr = &number_1;
                resultSign = sign1;
            }

            // меньшее из двух чисел переведем в доп код и сложим, затем
            // проставим знак
            sub_only_bits(*bigPtr, *smallPtr, result);
            set_scale(result, get_scale(&number_1));
            set_sign(result, resultSign);
        }
    }

    return return_value;
}

int s21_mul(s21_decimal number_1, s21_decimal number_2, s21_decimal *result) {
    clear_bits(result);
    int return_value = OK;
    int sign_result;

    if (get_sign(&number_1) != get_sign(&number_2)) {
        sign_result = 1;
    } else {
        sign_result = 0;
    }

    int last_bit_1 =
        last_bit(number_1);  // Находим последнюю единицу справа налево
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

void scale_equalize(s21_decimal *value_1, s21_decimal *value_2) {
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

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return (s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2));
}

int s21_is_less(s21_decimal dec1, s21_decimal dec2) {
    return s21_is_greater(dec2, dec1);
}

// 210                // 210
// 100                // 010
void div_only_bits(s21_decimal number_1, s21_decimal number_2, s21_decimal *buf,
                   s21_decimal *result) {
    clear_bits(buf);
    clear_bits(result);
    // 2
    for (int i = last_bit(number_1); i >= 0; i--) {    // i = 1
        if (get_bit(number_1, i)) set_bit(buf, 0, 1);  // buf == 010
        if (s21_is_greater_or_equal(*buf, number_2) == TRUE) {  // Прыгаем сюда
                                                                // 010  // 010
            sub_only_bits(*buf, number_2, buf);
            if (i != 0) offset_left(buf, 1);
            if (get_bit(number_1, i - 1)) set_bit(buf, 0, 1);
            offset_left(result, 1);
            set_bit(result, 0, 1);
        } else {
            offset_left(result, 1);           // result == 000
            if (i != 0) offset_left(buf, 1);  // buf == 010
            if ((i - 1) >= 0 && get_bit(number_1, i - 1)) set_bit(buf, 0, 1);
        }
    }
}

void bits_copy(s21_decimal src, s21_decimal *dest) {
    dest->bits[0] = src.bits[0];
    dest->bits[1] = src.bits[1];
    dest->bits[2] = src.bits[2];
}

int s21_div(s21_decimal divident, s21_decimal divisor, s21_decimal *result) {
    int return_value = OK;
    clear_bits(result);

    s21_decimal zero = {{0, 0, 0, 0}};

    if (s21_is_equal(divisor, zero) == TRUE) {
        //    +-x/0
        return_value = DIVISION_BY_ZERO;
        clear_bits(result);

    } else {
        int beginScale = get_scale(&divident) - get_scale(&divisor);
        int resultSign = get_sign(&divident) != get_sign(&divisor);

        s21_decimal remainder = {0}, tmp = {0};

        // для предсказуемости зачищаем
        set_scale(&divisor, 0);
        set_scale(&divident, 0);
        set_sign(&divisor, 0);
        set_sign(&divident, 0);

        // первое целочисленное деление
        // 100    // 10
        div_only_bits(divident, divisor, &remainder, &tmp);
        bits_copy(tmp, result);

        // 1/10 от макс значения децимал - граница от переполнения
        s21_decimal border_value = {{4294967295u, 4294967295, 4294967295u, 0}};
        s21_decimal ten = {{10, 0, 0, 0}};
        set_scale(&border_value, 1);

        int inside_scale = 0;

        // делим, пока не достигнем максимальной точности или пока не поделим
        // без остатка

        for (; inside_scale <= 27 && s21_is_equal(remainder, zero) == FALSE;) {
            if (s21_is_less(*result, border_value) == FALSE) {
                break;
            }
            s21_mul(remainder, ten, &remainder);
            div_only_bits(remainder, divisor, &remainder, &tmp);
            s21_mul(*result, ten, result);
            bit_addition(*result, tmp, result);
            inside_scale++;
        }

        s21_decimal musor;
        // вводим итоговый скейл в требуемые границы
        int endScale = beginScale + inside_scale;
        for (; endScale > 28;) {
            div_only_bits(*result, ten, &musor, result);
            endScale--;
        }
        for (; endScale < 0;) {
            s21_mul(*result, ten, result);
            endScale++;
        }

        set_scale(result, endScale);
        set_sign(result, resultSign);
    }

    return return_value;
}

int s21_truncate(s21_decimal number_1, s21_decimal *result) {
    s21_decimal ten = {{10, 0, 0, 0}};
    s21_decimal tmp = {{0, 0, 0, 0}};

    int sign_number_1 = get_sign(&number_1);
    int scale = get_scale(&number_1);

    if (!scale) {
        *result = number_1;
    } else {
        for (int i = scale; i > 0; i--) {
            div_only_bits(number_1, ten, &tmp, result);
            number_1 = *result;
        }
    }

    if (sign_number_1) set_sign(result, 1);

    return OK;
}

int s21_mod(s21_decimal number_1, s21_decimal number_2,
            s21_decimal *result) {  // нужны проверки
    int return_value = OK;
    clear_bits(result);
    s21_decimal zero = {{0, 0, 0, 0}};
    if (s21_is_equal(number_2, zero)) {
        return_value = DIVISION_BY_ZERO;
    }
    s21_decimal tmp;
    clear_bits(&tmp);  //  на всякий случай
    while (s21_is_greater(number_1, number_2)) {
        s21_sub(number_1, number_2, &number_1);
    }
    s21_div(number_1, number_2, &tmp);
    s21_truncate(tmp, &tmp);
    s21_mul(tmp, number_2, &tmp);
    s21_sub(number_1, tmp, result);
    return return_value;
}

// int main() {
//     s21_decimal value_1;
//     s21_decimal value_2;
//     s21_decimal result;
//     for (int i = 0; i < 4; i++) {
//         value_1.bits[i] = 0;
//         value_2.bits[i] = 0;
//         result.bits[i] = 0;
//     }
//     value_1.bits[0] = 45;
//     set_scale(&value_1, 1);
//     value_2.bits[0] = 3;
//     s21_mod(value_1, value_2, &result);
//     // printf("value_1.bits[0] == %u\n", value_1.bits[0]);
//     // printf("value_1.bits[1] == %x\n", value_1.bits[1]);
//     // printf("value_1.bits[2] == %x\n", value_1.bits[2]);
//     // printf("value_1.bits[3] == %x\n", value_1.bits[3]);
//     // printf("\n");
//     printf("result.bits[0] == %u\n", result.bits[0]);
//     printf("result.bits[1] == %x\n", result.bits[1]);
//     printf("result.bits[2] == %x\n", result.bits[2]);
//     printf("result.bits[3] == %x\n", result.bits[3]);
//     printf("result scale == %d\n", get_scale(&result));
//     printf("result sign == %d\n", get_sign(&result));
//     for (int i = 95; i >= 0; i--) {
//         printf("%d", get_bit(result, i));
//     }

//     return 0;
// }