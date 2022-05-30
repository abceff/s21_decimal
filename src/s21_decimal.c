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

/**
 * @brief Проверка больше ли первое число, чем второе
 * @param dec1 Первое число децимал
 * @param dec2 Второе число децимал
 * @return 0 - больше, 1 - меньше
 */
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
    int res;
    if (zero_check(value_1, value_2)) {
        res = FALSE;
    } else if (get_sign(&value_1) > get_sign(&value_2)) {
        res = FALSE;
    } else if (get_sign(&value_1) < get_sign(&value_2)) {
        res = TRUE;
    } else {
        if (get_scale(&value_1) < get_scale(&value_2)) {
            res = TRUE;
        } else if (get_scale(&value_1) > get_scale(&value_2)) {
            res = FALSE;
        } else {
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
    }

    if (get_sign(&value_1) && get_sign(&value_2))
        res = !res;

    return res;
}