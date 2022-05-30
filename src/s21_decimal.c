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

int super_get_bit(const super_s21_decimal decVar, int bit) {
    int res = 0;
    unsigned int mask = 1u << (bit % 32);
    res = decVar.bits[bit / 32] & mask;
    return !!res;
}

void super_set_bit(super_s21_decimal *varPtr, int bit, int value) {
    unsigned int mask = 1u << (bit % 32);
    if (value) {
        varPtr->bits[bit / 32] |= mask;
    } else if (!value) {
        varPtr->bits[bit / 32] &= ~mask;
    }
}

void super_offset_left(super_s21_decimal *varPtr, int value_offset) {
    for (int i = 0; i < value_offset; i++) {
        int value_31bit = super_get_bit(*varPtr, 31);
        int value_63bit = super_get_bit(*varPtr, 63);
        int value_95bit = super_get_bit(*varPtr, 95);
        int value_127bit = super_get_bit(*varPtr, 127);
        int value_159bit = super_get_bit(*varPtr, 159);
        int value_191bit = super_get_bit(*varPtr, 191);
        int value_223bit = super_get_bit(*varPtr, 223);
        int value_255bit = super_get_bit(*varPtr, 255);
        int value_287bit = super_get_bit(*varPtr, 287);
        int value_319bit = super_get_bit(*varPtr, 319);
        int value_351bit = super_get_bit(*varPtr, 351);
        varPtr->bits[0] <<= 1;
        varPtr->bits[1] <<= 1;
        varPtr->bits[3] <<= 1;
        varPtr->bits[4] <<= 1;
        varPtr->bits[5] <<= 1;
        varPtr->bits[6] <<= 1;
        varPtr->bits[7] <<= 1;
        varPtr->bits[8] <<= 1;
        varPtr->bits[9] <<= 1;
        varPtr->bits[10] <<= 1;
        varPtr->bits[11] <<= 1;
        if (value_31bit) super_set_bit(varPtr, 32, 1);
        if (value_63bit) super_set_bit(varPtr, 64, 1);
        if (value_95bit) super_set_bit(varPtr, 96, 1);
        if (value_127bit) super_set_bit(varPtr, 128, 1);
        if (value_159bit) super_set_bit(varPtr, 160, 1);
        if (value_191bit) super_set_bit(varPtr, 192, 1);
        if (value_223bit) super_set_bit(varPtr, 224, 1);
        if (value_255bit) super_set_bit(varPtr, 256, 1);
        if (value_287bit) super_set_bit(varPtr, 288, 1);
        if (value_319bit) super_set_bit(varPtr, 320, 1);
        if (value_351bit) super_set_bit(varPtr, 352, 1);
    }
}

void super_offset_right(super_s21_decimal *varPtr, int value_offset) {
    for (int i = 0; i < value_offset; i++) {
        int value_32bit = super_get_bit(*varPtr, 32);
        int value_64bit = super_get_bit(*varPtr, 64);
        int value_96bit = super_get_bit(*varPtr, 96);
        int value_128bit = super_get_bit(*varPtr, 128);
        int value_160bit = super_get_bit(*varPtr, 160);
        int value_192bit = super_get_bit(*varPtr, 192);
        int value_224bit = super_get_bit(*varPtr, 224);
        int value_256bit = super_get_bit(*varPtr, 256);
        int value_288bit = super_get_bit(*varPtr, 288);
        int value_320bit = super_get_bit(*varPtr, 320);
        int value_352bit = super_get_bit(*varPtr, 352);
        varPtr->bits[0] <<= 1;
        varPtr->bits[1] <<= 1;
        varPtr->bits[3] <<= 1;
        varPtr->bits[4] <<= 1;
        varPtr->bits[5] <<= 1;
        varPtr->bits[6] <<= 1;
        varPtr->bits[7] <<= 1;
        varPtr->bits[8] <<= 1;
        varPtr->bits[9] <<= 1;
        varPtr->bits[10] <<= 1;
        varPtr->bits[11] <<= 1;
        if (value_32bit) super_set_bit(varPtr, 31, 1);
        if (value_64bit) super_set_bit(varPtr, 63, 1);
        if (value_96bit) super_set_bit(varPtr, 95, 1);
        if (value_128bit) super_set_bit(varPtr, 127, 1);
        if (value_160bit) super_set_bit(varPtr, 159, 1);
        if (value_192bit) super_set_bit(varPtr, 191, 1);
        if (value_224bit) super_set_bit(varPtr, 223, 1);
        if (value_256bit) super_set_bit(varPtr, 255, 1);
        if (value_288bit) super_set_bit(varPtr, 287, 1);
        if (value_320bit) super_set_bit(varPtr, 319, 1);
        if (value_352bit) super_set_bit(varPtr, 351, 1);
    }
}

int last_bit(s21_decimal number) {
    int last_bit = 95;
    for (; last_bit >= 0 && get_bit(number, last_bit) == 0; last_bit--) {
    }
    return last_bit;
}

int super_last_bit(super_s21_decimal number) {
    int last_bit = 383;
    for (; last_bit >= 0 && super_get_bit(number, last_bit) == 0; last_bit--) {
    }
    return last_bit;
}

void super_bit_addition(super_s21_decimal var1, super_s21_decimal var2, super_s21_decimal* result) {
    int buffer = 0;

    for (int i = 0; i < 384; i++) {
        int cur_bit_of_var1 = super_get_bit(var1, i);
        int cur_bit_of_var2 = super_get_bit(var2, i);

        if (!cur_bit_of_var1 && !cur_bit_of_var2) {  // оба бита выключены
            if (buffer) {
                super_set_bit(result, i, 1);
                buffer = 0;
            } else {
                super_set_bit(result, i, 0);
            }
        } else if (cur_bit_of_var1 != cur_bit_of_var2) {
            if (buffer) {
                super_set_bit(result, i, 0);
                buffer = 1;
            } else {
                super_set_bit(result, i, 1);
            }
        } else {  // оба бита включены
            if (buffer) {
                super_set_bit(result, i, 1);
                buffer = 1;
            } else {
                super_set_bit(result, i, 0);
                buffer = 1;
            }
        }
    }
}

void from_super_decimal_to_decimal(super_s21_decimal super_decimal, s21_decimal* decimal){
    for (int i = 0; i < 4; i++) {
        decimal->bits[i] = super_decimal.bits[i];
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
    super_s21_decimal super_tmp_res;

    super_s21_decimal super_result = {0};

    // само умножение
    for (int i = 0; i <= last_bit_1; i++) {

        super_clear_bits(&super_tmp_res);
        int value_bit_1 = get_bit(value_1, i);

        if (value_bit_1) {
            for (int j = 0; j < 4; j++)
                super_tmp_res.bits[j] = value_2.bits[j];
            super_offset_left(&super_tmp_res, i);
            super_bit_addition(super_result, super_tmp_res, &super_result);
        }
    }

    int super_scale = get_scale(&value_1) + get_scale(&value_2);
    int super_result_last_bit = super_last_bit(super_result);

    // Если после умножения позиция последнего бита больше 95
    if (super_result_last_bit > 95) {
        return_value = INF;
        clear_bits(result);
    } else {
        from_super_decimal_to_decimal(super_result, result);

        set_scale(result, super_scale);
        set_sign(result, sign_result);
    }

    return return_value;
}

void clear_bits(s21_decimal *varPtr) {
    memset(varPtr->bits, 0, sizeof(varPtr->bits));
}

void super_clear_bits(super_s21_decimal *varPtr) {
    memset(varPtr->bits, 0, sizeof(varPtr->bits));
}

int main() {
    s21_decimal value_1 = {{2u, 0u, 0u, 0u}};
    s21_decimal value_2 = {{3u, 0u, 0u, 0u}};
    s21_decimal result = {{0u, 0u, 0u, 0u}};
    s21_mul(value_1, value_2, &result);
    printf("result.bits[0] == %d\n", result.bits[0]);
    return 0;
}