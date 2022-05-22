#include "s21_decimal.h"

s21_decimal get_mask(unsigned int index, unsigned int n,
                     unsigned int byte_number) {
    s21_decimal res;
    for (int i = 0; i < 4; i++) {
        res.bits[i] = 0;
    }
    
    if (index <= 31)  // 0 <= index <= 31
        byte_number = 0;
    else if (index <= 63)  // 32 <= index <= 63
        byte_number = 1;
    else  // 64 <= index <= 95
        byte_number = 2;

    res.bits[byte_number] = ~(4294967295 << n) << index;

    return res;
}

int get_bit(s21_decimal value, int index) {
    unsigned int byte_number;
    if (index <= 31)  // 0 <= index <= 31
        byte_number = 0;
    else if (index <= 63)  // 32 <= index <= 63
        byte_number = 1;
    else  // 64 <= index <= 95
        byte_number = 2;
    s21_decimal res;
    res.bits[byte_number] = value.bits[byte_number] &
                            get_mask(index, 1, byte_number).bits[byte_number];
    if (res.bits[byte_number] == get_mask(index, 1, byte_number).bits[byte_number])
        return 1;
    else
        return 0;
}

// int main() {
//     s21_decimal value;
//     for (int i = 0; i < 4; i++) {
//         value.bits[i] = 0;
//     }
//     value.bits[0] = 2;
//     printf("%d\n", get_bit(value, 1));
//     return 0;
// }