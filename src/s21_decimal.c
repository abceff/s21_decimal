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

int main() {
    s21_decimal value;
    for (int i = 0; i < 4; i++) {
        value.bits[i] = 0;
    }
    value.bits[0] = 3;
    set_bit(&value, 0, 0);
    printf("%d\n", value.bits[0]);
    return 0;
}
