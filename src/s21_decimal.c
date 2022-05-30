#include "s21_decimal.h"

/**
 * @brief Временная функция для инициализаци структуры которая хранит число
 * Децимал
 * @param varPtr указатель на число децимал
 */
void init_struct(s21_decimal *varPtr) {
    clear_bits(varPtr);
    // varPtr->value_type = s21_NORMAL_VALUE;
}

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
 * @brief побитовый сдвиг влево
 * @param varPtr указатель на число децимал
 * @param value_offset количество сдвигов влево
 */
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

/**
 * @brief
 * @param number
 * @return 0-95 - номер значащего бита, -1 - если все биты пустые
 */
int last_bit(s21_decimal number) {
    int last_bit = 95;
    for (; last_bit >= 0 && get_bit(number, last_bit) == 0; last_bit--) {
    }
    return last_bit;
}

/**
 * @brief Функция переводит число децимал в доп.код
 * @param value_1 указатель на число децимал
 */
void convert_to_addcode(s21_decimal *value_1) {
    s21_decimal result;
    s21_decimal add = {{1, 0, 0, 0}};

    value_1->bits[0] = ~value_1->bits[0];
    value_1->bits[1] = ~value_1->bits[1];
    value_1->bits[2] = ~value_1->bits[2];

    bit_addition(*value_1, add, &result, OK);

    value_1->bits[0] = result.bits[0];
    value_1->bits[1] = result.bits[1];
    value_1->bits[2] = result.bits[2];

}

/**
 * @brief Уравнивает степени двух чисел типа децимал
 * @param value_1 указатель на число децимал
 * @param value_2 указатель на число децимал
 * @return пока что под вопросом что нужно возвращать
 */
void scale_equalize(s21_decimal *value_1, s21_decimal *value_2) {
    s21_decimal *bigger = NULL;
    s21_decimal *smaller = NULL;
    bigger += 0;
    smaller += 0;

    if (get_scale(value_1) == get_scale(value_2)) {
        return;
    } else if (get_scale(value_1) > get_scale(value_2)) {
        bigger = value_1;
        smaller = value_2;
    } else {
        bigger = value_2;
        smaller = value_1;
    }

    s21_decimal tmp;
    init_struct(&tmp);

    int scaleSmall;
    // int scaleBig;

    int bit_addition_result = OK;
    int offset_left_result = OK;
    // уравнивание скейлов
    while (get_scale(value_1) != get_scale(value_2)) {
        // if (tmp.value_type == s21_NORMAL_VALUE) {
        //     // по умолчанию двигается в сторону увеличения скейла

        scaleSmall = get_scale(smaller);
        s21_decimal tmp1;
        s21_decimal tmp2;
        tmp1 = *smaller;
        tmp2 = *smaller;
        offset_left_result = offset_left(&tmp1, 1);
        offset_left_result = offset_left(&tmp2, 3);
        bit_addition_result = bit_addition(tmp1, tmp2, &tmp, offset_left_result);  // эквивалентно умножению на 10
        if (bit_addition_result == OK) {
            bits_copy(tmp, smaller);
            set_scale(smaller, scaleSmall + 1);
        }
        // } else {
        //     // в противном случае уменьшение скейла

        //     s21_decimal remainder;
        //     s21_decimal ten = {{10, 0, 0, 0}, s21_NORMAL_VALUE};

        //     s21_decimal tmpDiv = div_only_bits(*bigger, ten, &remainder);
        //     s21_decimal zero = {{0, 0, 0, 0}, s21_NORMAL_VALUE};
        //     if (zero_check(tmpDiv, zero) == 1) {
        //         // tmpDiv не обрезался по самые гланды
        //         bits_copy(tmpDiv, bigger);
        //     } else {
        //         // bigger при делении превратился в ноль - плохо
        //         bits_copy(remainder, bigger);
        //     }

        //     scaleBig = get_scale(bigger);
        //     set_scale(bigger, scaleBig - 1);
        // }
    }
    return;
}

/**
 * @brief складывает биты, не трогая скейл
 * @param var1 первое слагаемое
 * @param var2 второе слагаемое
 * @return s21_decimal результат сложения
 */
int bit_addition(s21_decimal var1, s21_decimal var2, s21_decimal* result, int offset_left_result) {
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
    if (offset_left_result == INF)
        return_value = INF;

    return return_value;
}

// /**
//  * @brief Предварительные проверки для check_for_add
//  *
//  * @param value_1 первое слагаемое
//  * @param value_2 второе слагаемое
//  * @return s21_decimal число в выставленным value_type
//  */
// s21_decimal check_for_add(s21_decimal value_1, s21_decimal value_2) {
//     s21_decimal res = {{0, 0, 0, 0}, s21_NORMAL_VALUE};
//     /*
//     num1      num2
//     +inf      +-normal  = +inf    - в простой проверке
//     -inf      +-normal  = -inf    - в простой проверке
//     nan       any       = nan     - в простой проверке
//     +inf      -inf      = nan     - отдельная
//     */

//     // простая проверка
//     res.value_type = value_1.value_type > value_2.value_type
//                          ? value_1.value_type
//                          : value_2.value_type;

//     if (value_1.value_type != s21_NORMAL_VALUE &&
//         value_2.value_type != s21_NORMAL_VALUE &&
//         value_1.value_type != value_2.value_type) {
//         res.value_type = s21_NAN;
//     }
//     return res;
// }

/**
 * @brief Сложение двух чисел децимал
 * @param value_1 число децимал
 * @param value_2 число децимал
 * @return число цимал
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = OK;
    // s21_decimal res = check_for_add(value_1, value_2);

    if (!get_sign(&value_1) && !get_sign(&value_2)) {
        //  оба числа положительных

        if (get_scale(&value_1) != get_scale(&value_2)) {
            scale_equalize(&value_1, &value_2);
        }

        int bit_additioin_result = OK;
        s21_decimal tmpRes;
        bit_additioin_result = bit_addition(value_1, value_2, &tmpRes, OK);

        if (bit_additioin_result == INF &&
            get_scale(&value_1) == 0) {
            // значит в результате бесконечность
            return_value = INF;

        } else if (bit_additioin_result == INF &&
                    get_scale(&value_1) > 0) {
            // можем понизить скейл
            while (bit_additioin_result == INF &&
                    (get_scale(&value_1) > 0 && get_scale(&value_2) > 0)) {
                // оба числа делим на 10, если позволяет скейл

                s21_decimal ten = {{10, 0, 0, 0}};
                s21_decimal remainder1, remainder2;
                s21_decimal tmpDiv1;
                div_only_bits(value_1, ten, &remainder1, &tmpDiv1);
                s21_decimal tmpDiv2;
                div_only_bits(value_2, ten, &remainder2, &tmpDiv2);

                // обезопасимся от обнуления
                s21_decimal zero = {{0, 0, 0, 0}};

                zero_check(tmpDiv1, zero) == 1
                    ? bits_copy(tmpDiv1, &value_1)
                    : bits_copy(remainder1, &value_1);
                zero_check(tmpDiv2, zero) == 1
                    ? bits_copy(tmpDiv2, &value_2)
                    : bits_copy(remainder2, &value_2);

                set_scale(&value_1, get_scale(&value_1) - 1);
                set_scale(&value_2, get_scale(&value_2) - 1);
                s21_add(value_1, value_2, result);
            }

        } else {
            // результат можно сразу присвоить
            *result = tmpRes;
            result->bits[3] = value_1.bits[3];
        }

    } else if (get_sign(&value_1) && !get_sign(&value_2)) {
        // 1 число отрецательное 2 число положительное
        // вызывается функция вычитания которая
        // создает доп код и сново вызывает сложение
        set_sign(&value_1, 0);
        s21_sub(value_2, value_1, result);

    } else if (!get_sign(&value_1) && get_sign(&value_2)) {
        // 1 полож 2 отрец
        set_sign(&value_2, 0);
        s21_sub(value_1, value_2, result);

    } else {
        // оба отрицательных
        set_sign(&value_1, 0);
        set_sign(&value_2, 0);
        s21_add(value_1, value_2, result);
        set_sign(result, 1);
        // if (res.value_type == s21_INFINITY) {
        //     res.value_type = s21_NEGATIVE_INFINITY;
        //     clear_bits(&res);
        // }
    }

    return return_value;
}

/**
 * @brief Разница чисел децимал
 * @param value_1 число децимал 1
 * @param value_2 число децимал 2
 * @return Возвращает разницу
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = OK;
    // s21_decimal res = {0, 0, 0, 0};

    if (get_scale(&value_1) != get_scale(&value_2)) {
        scale_equalize(&value_1, &value_2);
    }

    int resultSign;

    // проверяем на знаки
    if (get_sign(&value_1) != get_sign(&value_2)) {
        // знаки разные - ситуация вырождается в ++ или --
        resultSign = get_sign(&value_1);
        set_sign(&value_1, 0);
        set_sign(&value_2, 0);
        s21_add(value_1, value_2, result);
        set_sign(result, resultSign);

    } else {
        // знаки одинаковые - ситуация вырождается в -+ или +-
        if (s21_is_equal(value_1, value_2) == TRUE) {
            // они равны, все ок, ничего не делаем, в результате уже лежит 0

        } else {
            // числа разные, значит нужно знать какой знак проставлять и тд
            int sign1 = get_sign(&value_1);
            int sign2 = get_sign(&value_2);
            set_sign(&value_1, 0);
            set_sign(&value_2, 0);
            s21_decimal *smallPtr, *bigPtr;

            // кто из них больше по модулю
            if (s21_is_less(value_1, value_2) == TRUE) {
                smallPtr = &value_1;
                bigPtr = &value_2;
                resultSign = !sign2;
            } else {
                smallPtr = &value_2;
                bigPtr = &value_1;
                resultSign = sign1;
            }

            // меньшее из двух чисел переведем в доп код и сложим, затем
            // проставим знак
            convert_to_addcode(smallPtr);
            s21_add(*smallPtr, *bigPtr, result);
            set_sign(result, resultSign);
        }
    }


    return return_value;
}

/**
 * @brief Проверка аргументов перед умножением
 *
 * @param value_1 первый множитель
 * @param value_2 второй множитель
 * @return int число, в котором выставлен соотв. флаг
 */

// int check_for_mul(s21_decimal value_1, s21_decimal value_2) {
//     int return_value = OK;
//     // s21_decimal res = {{0, 0, 0, 0}, s21_NORMAL_VALUE};

//     // проверки аргументов
    
//     // int there_is_nan = is_NAN(&value_1, &value_2) == TRUE ? 1 : 0;  // У нас число не может быть nan
//     // int there_is_neg_inf = is_neg_inf(&value_1, &value_2) != 0 ? 1 : 0;
//     // int there_is_inf = is_inf(&value_1, &value_2) != 0 ? 1 : 0;
//     s21_decimal zero = {0, 0, 0, 0};
//     int there_is_zero = s21_is_equal(value_1, zero) == TRUE ||
//                                 s21_is_equal(value_2, zero) == TRUE
//                             ? 1
//                             : 0;

//     int there_is_plus_normal =
//         (s21_is_greater(value_1, zero) == TRUE) ||
//                 (s21_is_greater(value_2, zero) == TRUE)
//             ? 1
//             : 0;

//     int there_is_neg_normal = (s21_is_less(value_1, zero) == TRUE) ||
//                                       (s21_is_less(value_2, zero) == TRUE)
//                                   ? 1
//                                   : 0;

//     // res.value_type =
//     //     value_1.value_type ? value_1.value_type : value_2.value_type;

//     // if (there_is_nan || (there_is_zero && there_is_inf) ||
//     //     (there_is_zero && there_is_neg_inf)) {
//     //     res.value_type = s21_NAN;
//     // } else 

//     return res;
// }

void from_decimal_to_super_decimal(s21_decimal* decimal, super_s21_decimal* super_decimal){
    for (int i = 0; i < 4; i++) {
        super_decimal->bits[i] = decimal->bits[i];
    }
}

/**
 * @brief функция умножение двух чисел децимал
 * @param value_1
 * @param value_2
 * @return s21_decimal
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = OK;
    int sign_result;

    if (get_sign(&value_1) != get_sign(&value_2)) {
        sign_result = 1;
    } else {
        sign_result = 0;
    }

    int last_bit_1 = last_bit(value_1);  // Находим последнюю единицу справа налево
    super_s21_decimal tmp_res;

    super_s21_decimal result = {0};

    // само умножение
    for (int i = 0; i <= last_bit_1; i++) {

        clear_bits(&tmp_res);
        int value_bit_1 = get_bit(value_1, i);

        if (value_bit_1) {
            for (int j = 0; j < 4; j++)
                tmp_res.bits[j] = value_2.bits[j];
            offset_left(&tmp_res, i);
            bit_addition(*result, tmp_res, result, offset_left_result);
        }
    }

    if (bit_addition_result == INF)
        return_value = INF;

    int scale = get_scale(&value_1) + get_scale(&value_2);
    set_scale(result, scale);
    set_sign(result, sign_result);

    if (return_value != OK) {
        clear_bits(result);
    }

    return return_value;
}

// /**
//  * @brief Является ли одно из чисел NAN
//  * @param dec1 Первое число децимал
//  * @param dec2 Второе число децимал
//  * @return FALSE - числа не NAN, TRUE - одно из децималов NAN
//  */
// int is_NAN(s21_decimal *dec1, s21_decimal *dec2) {
//     int vt_dec1 = dec1->value_type;
//     int vt_dec2 = dec2->value_type;

//     return (vt_dec1 == s21_NAN || vt_dec2 == s21_NAN) ? TRUE : FALSE;
// }

// /**
//  * @brief Проверка на полож бесконечность
//  * @param dec1 Первое число децимал
//  * @param dec2 Второе число децимал
//  * @return 0 - не бесконечны, 1 - первое бесконечно, 2 - оба бесконечны, -1 -
//  * второе бесконечно
//  */
// int is_inf(s21_decimal *dec1, s21_decimal *dec2) {
//     int who_is_inf = 0;
//     int vt_dec1 = dec1->value_type;
//     int vt_dec2 = dec2->value_type;

//     if (vt_dec1 == s21_INFINITY && vt_dec2 != s21_INFINITY) {
//         who_is_inf = 1;
//     } else if (vt_dec1 != s21_INFINITY && vt_dec2 == s21_INFINITY) {
//         who_is_inf = -1;
//     } else if (vt_dec1 == s21_INFINITY && vt_dec2 == s21_INFINITY) {
//         who_is_inf = 2;
//     }
//     return who_is_inf;
// }

// /**
//  * @brief Проверка на отриц бесконечность
//  * @param dec1 Первое число децимал
//  * @param dec2 Второе число децимал
//  * @return 0 - не бесконечны, 1 - первое бесконечно, 2 - оба бесконечны, -1 -
//  * второе бесконечно
//  */
// int is_neg_inf(s21_decimal *dec1, s21_decimal *dec2) {
//     int who_is_inf = 0;
//     int vt_dec1 = dec1->value_type;
//     int vt_dec2 = dec2->value_type;

//     if (vt_dec1 == s21_NEGATIVE_INFINITY && vt_dec2 != s21_NEGATIVE_INFINITY) {
//         who_is_inf = 1;
//     }
//     if (vt_dec1 != s21_NEGATIVE_INFINITY && vt_dec2 == s21_NEGATIVE_INFINITY) {
//         who_is_inf = -1;
//     }
//     if (vt_dec1 == s21_NEGATIVE_INFINITY && vt_dec2 == s21_NEGATIVE_INFINITY) {
//         who_is_inf = 2;
//     }

//     return who_is_inf;
// }

/**
 * @brief Проверка чисел на отрицательное значение
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - оба одного знака, 1 - первое положительное, -1 - второе
 * положительное
 */
int is_negative(s21_decimal *dec1, s21_decimal *dec2) {
    int who_is_negative = 0;
    int sign_dec1 = get_sign(dec1);
    int sign_dec2 = get_sign(dec2);

    if (!sign_dec1 && sign_dec2) who_is_negative = 1;
    if (sign_dec1 && !sign_dec2) who_is_negative = -1;

    return who_is_negative;
}

/**
 * @brief Проверка скейла чисел и приведение к одному
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 */
void check_scale(s21_decimal *dec1, s21_decimal *dec2) {
    int scale_dec1 = get_scale(dec1);
    int scale_dec2 = get_scale(dec2);

    if (scale_dec1 != scale_dec2) {
        scale_equalize(dec1, dec2);
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

/**
 * @brief Проверка больше ли первое число, чем второе
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - больше, 1 - меньше
 */
int s21_is_greater(s21_decimal dec1, s21_decimal dec2) {
    int is_greater = -1;
    if (zero_check(dec1, dec2))
        is_greater = FALSE;

    if (is_greater == -1) {
        if (get_sign(&dec1) && !get_sign(&dec2)) {
            is_greater = FALSE;
        } else if (!get_sign(&dec1) && get_sign(&dec2)) {
            is_greater = TRUE;
        }
    }

    if (is_greater == -1) {
        check_scale(&dec1, &dec2);
    }

    for (int i = 95; i >= 0 && is_greater == -1; i--) {
        int bit_dec1 = get_bit(dec1, i);
        int bit_dec2 = get_bit(dec2, i);
        if (bit_dec1 && !bit_dec2) is_greater = TRUE;
        if (bit_dec2 && !bit_dec1) is_greater = FALSE;
    }

    if (get_sign(&dec1) && get_sign(&dec2)) is_greater = !is_greater;

    if (is_greater == -1) is_greater = FALSE;

    return is_greater;
}

/**
 * @brief Хитрая функция меньше ли первое число, чем второе
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - меньше, 1 - больше
 */
int s21_is_less(s21_decimal dec1, s21_decimal dec2) {
    return s21_is_greater(dec2, dec1);
}

/**
 * @brief Равны ли два числа децимал
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - равны, 1 - разные
 */
int s21_is_equal(s21_decimal dec1, s21_decimal dec2) {
    int is_equal = -1;

    if (is_equal == -1) {
        if (zero_check(dec1, dec2) == TRUE) is_equal = TRUE;
    }

    // if (is_equal == -1) {
    //     int who_is_negative = is_negative(&dec1, &dec2);
    //     if (who_is_negative != 0) is_equal = FALSE;

    //     check_scale(&dec1, &dec2);
    // }

    for (int i = 95; i >= 0 && is_equal == -1; i--) {
        int bit_dec1 = get_bit(dec1, i);
        int bit_dec2 = get_bit(dec2, i);
        if (bit_dec1 != bit_dec2) is_equal = FALSE;
    }

    return (is_equal != FALSE) ? TRUE : FALSE;
}

/**
 * @brief Хитрая - меньше или равно первое децимал второму
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - равны или меньше, 1 - больше
 */
int s21_is_less_or_equal(s21_decimal dec1, s21_decimal dec2) {
    return !(!s21_is_less(dec1, dec2) || !s21_is_equal(dec1, dec2));
}

/**
 * @brief Хитрая - больше или равно первое децимал второму
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - больше или равно, 1 - меньше
 */
int s21_is_greater_or_equal(s21_decimal dec1, s21_decimal dec2) {
    return !(!s21_is_greater(dec1, dec2) || !s21_is_equal(dec1, dec2));
}

/**
 * @brief Хитрая - не равно первое децимал второму
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - разные, 1 - равны
 */
int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2) {
    return (!s21_is_equal(dec1, dec2));
}

/**
 * @brief копирует только 96 бит
 * @param src откуда
 * @param dest куда
 */
void bits_copy(s21_decimal src, s21_decimal *dest) {
    dest->bits[0] = src.bits[0];
    dest->bits[1] = src.bits[1];
    dest->bits[2] = src.bits[2];
}

// /**
//  * @brief деление decimal
//  * @param divident делимое
//  * @param divisor делитель
//  * @return s21_decimal результат деления
//  */
// s21_decimal s21_div(s21_decimal divident, s21_decimal divisor) {
//     s21_decimal result;
//     init_struct(&result);

//     s21_decimal zero = {{0, 0, 0, 0}, s21_NORMAL_VALUE};
//     int divsr_is_normal_plus = s21_is_greater(divisor, zero) == TRUE &&
//                                        divisor.value_type == s21_NORMAL_VALUE
//                                    ? 1
//                                    : 0;

//     int divsr_is_normal_minus = s21_is_less(divisor, zero) == TRUE &&
//                                         divisor.value_type == s21_NORMAL_VALUE
//                                     ? 1
//                                     : 0;

//     int flag = 1;  // для отлова ситуации с normal/+-inf

//     if (is_NAN(&divident, &divisor) == TRUE) {
//         result.value_type = s21_NAN;

//     } else if (s21_is_equal(divisor, zero) == TRUE &&
//                s21_is_less(divident, zero) == TRUE) {
//         //    -x/0
//         result.value_type = s21_NEGATIVE_INFINITY;

//     } else if (s21_is_equal(divisor, zero) == TRUE &&
//                s21_is_greater(divident, zero) == TRUE) {
//         //   +x/0
//         result.value_type = s21_INFINITY;

//     } else if (s21_is_equal(divisor, zero) == TRUE &&
//                s21_is_equal(divident, zero) == TRUE) {
//         //    0/0
//         result.value_type = s21_NAN;

//     } else if ((divident.value_type == s21_INFINITY ||
//                 divident.value_type == s21_NEGATIVE_INFINITY) &&
//                (divisor.value_type == s21_INFINITY ||
//                 divisor.value_type == s21_NEGATIVE_INFINITY)) {
//         //    +-inf/(+-inf)
//         result.value_type = s21_NAN;

//     } else if ((divident.value_type == s21_INFINITY && divsr_is_normal_plus) ||
//                (divident.value_type == s21_NEGATIVE_INFINITY &&
//                 divsr_is_normal_minus)) {
//         //    +inf / +normal
//         //    -inf / -normal
//         result.value_type = s21_INFINITY;

//     } else if ((divident.value_type == s21_INFINITY && divsr_is_normal_minus) ||
//                (divident.value_type == s21_NEGATIVE_INFINITY &&
//                 divsr_is_normal_plus)) {
//         //    +inf / -normal = -inf
//         //    -inf / +normal
//         result.value_type = s21_NEGATIVE_INFINITY;

//     } else if (divident.value_type == s21_NORMAL_VALUE &&
//                (divisor.value_type == s21_INFINITY ||
//                 divisor.value_type == s21_NEGATIVE_INFINITY)) {
//         //    +-normal / +-inf = 0
//         result = zero;
//         flag = 0;
//     }

//     if (result.value_type == s21_NORMAL_VALUE && flag) {
//         int beginScale = get_scale(&divident) - get_scale(&divisor);
//         int resultSign = get_sign(&divident) != get_sign(&divisor);

//         s21_decimal remainder, tmp;

//         // для предсказуемости зачищаем
//         set_scale(&divisor, 0);
//         set_scale(&divident, 0);
//         set_sign(&divisor, 0);
//         set_sign(&divident, 0);

//         // первое целочисленное деление
//         tmp = div_only_bits(divident, divisor, &remainder);
//         bits_copy(tmp, &result);

//         // 1/10 от макс значения децимал - граница от переполнения
//         s21_decimal border_value = {{-1, -1, -1, 0}, s21_NORMAL_VALUE};
//         s21_decimal ten = {{10, 0, 0, 0}, s21_NORMAL_VALUE};

//         set_scale(&border_value, 1);
//         int inside_scale = 0;

//         // делим, пока не достигнем максимальной точности или пока не поделим
//         // без остатка

//         for (; inside_scale <= 27 && s21_is_equal(remainder, zero) == FALSE;) {
//             if (s21_is_less(result, border_value) == FALSE) {
//                 break;
//             }
//             remainder = s21_mul(remainder, ten);
//             tmp = div_only_bits(remainder, divisor, &remainder);
//             result = s21_mul(result, ten);
//             result = s21_add(result, tmp);
//             inside_scale++;
//         }

//         s21_decimal musor;
//         // вводим итоговый скейл в требуемые границы
//         int endScale = beginScale + inside_scale;
//         for (; endScale > 28;) {
//             div_only_bits(result, ten, &musor, &result);
//             endScale--;
//         }
//         for (; endScale < 0;) {
//             result = s21_mul(result, ten);
//             endScale++;
//         }

//         set_scale(&result, endScale);
//         set_sign(&result, resultSign);
//     }

//     return result;
// }

// /**
//  * @brief Конвертирует цисло из int в decimal
//  * @param src Число типа int
//  * @param dst Структура decimal'а
//  * @return Результат работы, где 0 - это false, а 1 - это true
//  */
// int s21_from_int_to_decimal(int src, s21_decimal *dst) {
//     char result = TRUE;
//     if (dst) {
//         dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
//         dst->value_type = 0;
//         if (src < 0) {
//             set_sign(dst, 1);
//             src *= -1;
//         }
//         dst->bits[0] = src;
//         dst->value_type = s21_NORMAL_VALUE;
//     } else {
//         result = FALSE;
//     }
//     return result;
// }

// /**
//  * @brief Конвертирует цисло из decimal в int
//  * @param src Число decimal
//  * @param dst Указатель на int
//  * @return Результат работы, где 0 - это false, а 1 - это true
//  */
// int s21_from_decimal_to_int(s21_decimal src, int *dst) {
//     int result = 1;
//     if (src.value_type == s21_NORMAL_VALUE) {
//         *dst = src.bits[0];
//         *dst *= get_sign(&src) ? -1 : 1;
//         *dst /= (int)pow(10, get_scale(&src));
//         result = 0;
//     }
//     return result;
// }

// /**
//  * @brief Получает знак в числе float
//  * @param src Цисло float
//  * @return Знак float'a
//  */
// int getFloatSign(float *src) { return *(int *)src >> 31; }

// /**
//  * @brief Получает экспоненту из float'a
//  * @param src число float
//  * @return Возвращает экспоненту
//  */
// int getFloatExp(float *src) { return ((*(int *)src & ~SIGN) >> 23) - 127; }

// /**
//  * @brief Делает перевод из float в децимал
//  * @param src Значение float
//  * @param dst Значение децимал
//  * @return Результат выполнения функции
//  */
// int s21_from_float_to_decimal(float src, s21_decimal *dst) {
//     dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
//     dst->value_type = s21_NORMAL_VALUE;
//     int result = FALSE, sign = getFloatSign(&src), exp = getFloatExp(&src);

//     if (isinf(src) && !sign)
//         dst->value_type = s21_INFINITY;
//     else if (isinf(src) && sign)
//         dst->value_type = s21_NEGATIVE_INFINITY;
//     else if (isnan(src))
//         dst->value_type = s21_NAN;

//     if (dst && dst->value_type == s21_NORMAL_VALUE && src != 0) {
//         double temp = (double)fabs(src);
//         int off = 0;
//         for (; off < 28 && (int)temp / (int)pow(2, 21) == 0;
//              temp *= 10, off++) {
//         }
//         temp = round(temp);
//         if (off <= 28 && (exp > -94 && exp < 96)) {
//             floatbits mant;
//             temp = (float)temp;
//             for (; fmod(temp, 10) == 0 && off > 0; off--, temp /= 10) {
//             }
//             mant.fl = temp;
//             exp = getFloatExp(&mant.fl);
//             dst->bits[exp / 32] |= 1 << exp % 32;
//             for (int i = exp - 1, j = 22; j >= 0; i--, j--)
//                 if ((mant.ui & (1 << j)) != 0) dst->bits[i / 32] |= 1 << i % 32;
//             dst->bits[3] = (sign << 31) | (off << 16);
//             result = TRUE;
//         }
//     }
//     return result;
// }

// /**
//  * @brief Делает перевод из децимала в float
//  * @param src Цисло децимал
//  * @param dst Цисло float
//  * @return Результат выполнения функции
//  */
// int s21_from_decimal_to_float(s21_decimal src, float *dst) {
//     int result = FALSE;
//     if (src.value_type == s21_NORMAL_VALUE) {
//         double temp = 0;
//         int off = 0;
//         for (int i = 0; i < 96; i++)
//             if ((src.bits[i / 32] & (1 << i % 32)) != 0) temp += pow(2, i);
//         if ((off = (src.bits[3] & ~SIGN) >> 16) > 0) {
//             for (int i = off; i > 0; i--) temp /= 10.0;
//         }
//         *dst = (float)temp;
//         *dst *= src.bits[3] >> 31 ? -1 : 1;
//         result = TRUE;
//     }
//     return result;
// }

/**
 * @brief Деление битов игнорируется степень
 * @param a число децимал
 * @param b число децимал
 * @param buf остаток от деления
 */
void div_only_bits(s21_decimal value_1, s21_decimal value_2,
                          s21_decimal *buf, s21_decimal* result) {
    init_struct(buf);
    for (int i = last_bit(value_1); i >= 0; i--) {
        if (get_bit(value_1, i)) set_bit(buf, 0, 1);
        if (s21_is_greater_or_equal(*buf, value_2) == TRUE) {
            s21_sub(*buf, value_2, buf);
            if (i != 0) offset_left(buf, 1);
            if (get_bit(value_1, i - 1)) set_bit(buf, 0, 1);
            offset_left(result, 1);
            set_bit(result, 0, 1);
        } else {
            offset_left(result, 1);
            if (i != 0) offset_left(buf, 1);
            if ((i - 1) >= 0 && get_bit(value_1, i - 1)) set_bit(buf, 0, 1);
        }
    }
}

// /**
//  * @brief Функция взятия остатка от деления
//  * @param value_1
//  * @param value_2
//  * @return s21_decimal
//  */
// s21_decimal s21_mod(s21_decimal value_1,
//                     s21_decimal value_2) {  // нужны проверки
//     s21_decimal res;
//     s21_decimal tmp;
//     init_struct(&tmp);  //  на всякий случай
//     tmp = s21_div(value_1, value_2);
//     tmp = s21_truncate(tmp);
//     tmp = s21_mul(tmp, value_2);
//     res = s21_sub(value_1, tmp);
//     return res;
// }

// /**
//  * @brief возвращает целую часть числа децимал
//  * @param a число децимал
//  * @return число децимал
//  */
// s21_decimal s21_truncate(s21_decimal value_1) {
//     s21_decimal ten = {{10, 0, 0, 0}, s21_NORMAL_VALUE};
//     s21_decimal res = {{0, 0, 0, 0}, 0};
//     s21_decimal tmp = {{0, 0, 0, 0}, s21_NORMAL_VALUE};

//     int sign_value_1 = get_sign(&value_1);
//     int scale = get_scale(&value_1);

//     int valid_value = (value_1.value_type == s21_NORMAL_VALUE ? 1 : 0);

//     if (!scale && valid_value) {
//         res = value_1;
//     } else if (valid_value) {
//         for (int i = scale; i > 0; i--) {
//             res = div_only_bits(value_1, ten, &tmp);
//             value_1 = res;
//         }
//     } else {
//         res = value_1;
//     }

//     if (sign_value_1 && valid_value) set_sign(&res, 1);

//     return res;
// }

// /**
//  * @brief Округление до целого числа
//  * @param dec1 Число к округлению
//  * @return Округленный децимал
//  */
// s21_decimal s21_round(s21_decimal dec1) {
//     int valid_value = (dec1.value_type == s21_NORMAL_VALUE ? 1 : 0);

//     s21_decimal res = {{0, 0, 0, 0}, s21_NORMAL_VALUE};
//     s21_decimal one = {{1, 0, 0, 0}, s21_NORMAL_VALUE};
//     s21_decimal five = {{5, 0, 0, 0}, s21_NORMAL_VALUE};

//     int sign = get_sign(&dec1);
//     set_sign(&dec1, 0);

//     s21_decimal trunc = s21_truncate(dec1);
//     s21_decimal buf = s21_sub(dec1, trunc);

//     set_scale(&five, 1);

//     if (valid_value) {
//         res = trunc;
//         if (s21_is_greater_or_equal(buf, five) == TRUE) {
//             res = s21_add(res, one);
//         }
//         set_sign(&res, sign);
//     } else {
//         res.value_type = dec1.value_type;
//     }

//     return res;
// }

// /**
//  * @brief Округление в сторону минус бесконечности
//  * @param dec1 Число к округлению
//  * @return Округленный децимал
//  */
// s21_decimal s21_floor(s21_decimal dec1) {
//     s21_decimal dec1_copy = dec1;
//     int valid_value = (dec1.value_type == s21_NORMAL_VALUE ? 1 : 0);
//     int sign_dec1 = get_sign(&dec1);
//     int scale_dec1 = get_scale(&dec1);

//     s21_decimal one = {{1, 0, 0, 0}, s21_NORMAL_VALUE};
//     s21_decimal ten = {{10, 0, 0, 0}, s21_NORMAL_VALUE};
//     s21_decimal buf;
//     init_struct(&buf);
//     for (int i = scale_dec1; i > 0; i--) dec1 = div_only_bits(dec1, ten, &buf);
//     set_scale(&dec1, 0);

//     if (s21_is_equal(dec1, dec1_copy) == TRUE) valid_value = 0;
//     if (sign_dec1 && valid_value) {
//         dec1 = s21_add(dec1, one);
//         set_sign(&dec1, 1);
//     }

//     return dec1;
// }

// /**
//  * @brief Смена знака числа
//  * @param dec1 Число, которому меняют знак
//  * @return Число с измененным знаком
//  */
// s21_decimal s21_negate(s21_decimal dec1) {
//     int vt_dec1 = dec1.value_type;

//     if (vt_dec1 != s21_NAN) {
//         int sign = get_sign(&dec1);

//         if (!sign) set_sign(&dec1, 1);
//         if (sign) set_sign(&dec1, 0);

//         if (vt_dec1 == s21_INFINITY) {
//             dec1.value_type = s21_NEGATIVE_INFINITY;
//             set_sign(&dec1, 1);
//         }

//         if (vt_dec1 == s21_NEGATIVE_INFINITY) {
//             dec1.value_type = s21_INFINITY;
//             set_sign(&dec1, 0);
//         }
//     }
//     return dec1;
// }

/**
 * @brief зануляет все биты передаваемой переменной,
 * не трогает скейл и тип
 *
 * @param varPtr указатель на очищаемую переменную
 */
void clear_bits(s21_decimal *varPtr) {
    memset(varPtr->bits, 0, sizeof(varPtr->bits));
}

// int main() {
//     s21_decimal value_1 = {{123456u, 654u, 0xFFFFFFFF, 00000000u}};
//     s21_decimal value_2 = {{123456u, 654u, 0xFFFFFFFF, 00000000u}};
//     set_scale(&value_2, 2);
//     int is_greater = s21_is_greater(value_1, value_2);
//     printf("is greater == %d\n", is_greater);
//     return 0;
// }