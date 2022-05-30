#include "s21_decimal.h"

#define MAX 2147483648u

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

int get(s21_decimal value, int index) {
  unsigned int element_number = get_element_number(index);
  s21_decimal res;
  res.bits[element_number] = value.bits[element_number] & get_mask(index, 1);
  return res.bits[element_number] ? 1 : 0;
}

void set(s21_decimal* value, int index, int set_value) {
  unsigned int element_number = get_element_number(index);
  if (set_value)
    value->bits[element_number] =
        value->bits[element_number] | get_mask(index, 1);
  else
    value->bits[element_number] =
        value->bits[element_number] & ~get_mask(index, 1);
}

//получаем значение плавающей точки
char s21_scale(s21_decimal value) { return (char)(value.bits[3] >> 16); }

int s21_scale_align(s21_decimal* value_1, s21_decimal* value_2,
                    int* return_value_int, s21_decimal* result) {
  int sign_value_1 = s21_sign(*value_1);
  int sign_value_2 = s21_sign(*value_2);
  int scale_value_1 = s21_scale(*value_1);
  int scale_value_2 = s21_scale(*value_2);
  int scale_res = scale_value_1;
  int flag = 0;
  s21_decimal temp = {{0, 0, 0, 0}};
  if (scale_value_1 > scale_value_2) {
    scale_res = scale_value_2;
    for (int i = 0; i < abs(scale_value_1 - scale_value_2); i++) {
      temp = s21_mul_simple(*value_2);
      if (temp.bits[3] != 0) {
        flag = 1;
        break;
      }
      *value_2 = temp;
      scale_res++;
    }
  } else if (scale_value_1 < scale_value_2) {
    scale_res = scale_value_1;
    for (int i = 0; i < abs(scale_value_1 - scale_value_2); i++) {
      temp = s21_mul_simple(*value_1);
      if (temp.bits[3] != 0) {
        flag = -1;
        break;
      }
      *value_1 = temp;
      scale_res++;
    }
  }
  if (flag > 0) {
    for (int i = 0; i < abs(scale_res - scale_value_1); i++) {
      *value_1 = dec_div_by_ten(*value_1, return_value_int, result);
    }
  } else if (flag < 0) {
    for (int i = 0; i < abs(scale_res - scale_value_2); i++) {
      *value_2 = dec_div_by_ten(*value_2, return_value_int, result);
    }
  }
  s21_set_scale(value_1, scale_res);
  s21_set_scale(value_2, scale_res);
  if (sign_value_1) s21_set_sign(value_1);
  if (sign_value_2) s21_set_sign(value_2);
  return scale_res;
}

int s21_sign(s21_decimal value) {
  unsigned int mask = MAX;
  return !!(mask & value.bits[3]);
}

void s21_set_scale(s21_decimal* dec, int scale) {
  char* p_to_scale = (char*)&(dec->bits[3]) + 2 * sizeof(char);
  *p_to_scale = (char)scale;
}

// выставление знака
void s21_set_sign(s21_decimal* dec) {
  unsigned int mask = MAX;
  dec->bits[3] |= mask;
}

s21_decimal dec_div_by_ten(s21_decimal src, int* return_value_int,
                           s21_decimal* result) {
  s21_decimal tmp = {{0, 0, 0, 0}};
  s21_decimal res = {{0, 0, 0, 0}};
  s21_decimal ten = {{0, 0, 0, 0}};
  unsigned int tmp_bit = 1;
  ten.bits[0] = 10;
  for (int i = 2; i >= 0; i--) {
    unsigned int src_bit = 0x80000000;
    while (src_bit) {
      unsigned int shift = src.bits[i] & src_bit;
      s21_shift_left(&tmp);
      if (src_bit == shift) {
        tmp.bits[0] |= tmp_bit;
      }
      if (tmp.bits[0] >= 10) {
        s21_shift_left(&res);
        res.bits[0] |= tmp_bit;
        tmp = s21_sub_simple(tmp, ten, return_value_int, result);
      } else {
        s21_shift_left(&res);
      }
      src_bit >>= 1;
    }
  }
  s21_decimal s21_one = {{0, 0, 0, 0}};
  s21_one.bits[0] = 1;
  if (res.bits[0] % 2 == 0) {
    if (tmp.bits[0] > 5) {
      s21_add_simple(res, s21_one, return_value_int, &res);
    }
  } else {
    if (tmp.bits[0] >= 5) s21_add_simple(res, s21_one, return_value_int, &res);
  }
  return res;
}

s21_decimal s21_sub_simple(s21_decimal x, s21_decimal y, int* return_value_int,
                           s21_decimal* result) {
  s21_decimal res = {{0, 0, 0, 0}};
  s21_invert(y, return_value_int, result);
  s21_add_simple(x, y, return_value_int, &res);
  return res;
}

s21_decimal* s21_shift_left(s21_decimal* x) {
  for (int i = 2; i >= 0; i--) {
    if (i != 2) {
      unsigned int temp = x->bits[i] & MAX;
      x->bits[i] <<= 1;
      if (temp == MAX) {
        x->bits[i + 1] |= 1;
      }
    } else {
      x->bits[i] <<= 1;
    }
  }
  return x;
}

void s21_invert(s21_decimal value_1, int* return_value_int,
                s21_decimal* result) {
  for (int i = 0; i < 3; i++) {
    value_1.bits[i] = ~value_1.bits[i];
  }
  s21_decimal term = {{0, 0, 0, 0}};
  term.bits[0] = 1;
  s21_add_simple(value_1, term, return_value_int, result);
}

/* проверка на положительный децимал */
int s21_is_positive(s21_decimal value) {
  int true = 0;
  if (!(value.bits[3] & MAX)) true = 1;
  return true;
}

void s21_add_simple(s21_decimal value_1, s21_decimal value_2,
                    int* return_value_int, s21_decimal* result) {
  // s21_decimal result = {{0, 0, 0, 0}};
  return_value_int += 0;
  int tmp = 0;  //тут будет 1 если мы складываем 1+1 бит и переносим 1 в
                //следующий разряд
  //проходимся по всем битам всех разрядов каждого числа
  for (int i = 0; i < 96; i++) {
    int bit_value_1 = get(value_1, i);
    int bit_value_2 = get(value_2, i);
    if (!bit_value_1 && !bit_value_2) {  //оба бита равны нулю
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 1
        set(result, i, 1);
        tmp = 0;
      } else {
        set(result, i, 0);  //записываем в результат 0, т.к. 0+0=0
      }
    } else if (bit_value_1 && bit_value_2) {  //оба бита 1
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 1
        set(result, i, 1);
        tmp = 1;  //тоже равен 1, т.к. остается от текущей операции
      } else {
        set(result, i, 0);
        tmp = 1;
      }
    } else {      //один равен 1
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 0 и переносим 1 (т.к. 1+1 = 10)
        set(result, i, 0);
        tmp = 1;
      } else {
        set(result, i, 1);
      }
    }
    if (i == 95 && tmp) {
      int a = 1;
      return_value_int = &a;
    }  //дошли до последнего бита и остается лишний бит
       //= происхоит переполнение
  }
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int return_value_int = 0;  //тут я что-то крышой поехала и мне кажется это
                             //по-другому можно реализовать но пока так будет
  //не дает внутри функкции просто инф присваивать инту*
  int return_value = s21_OK;
  // s21_decimal result = {{0, 0, 0, 0}};
  int scale = 0;
  int is_greater =
      is_greater_module(value_1, value_2, &return_value_int, result);
  if (s21_is_positive(value_1) & s21_is_positive(value_2)) {
    scale = s21_scale_align(&value_1, &value_2, &return_value_int, result);
    s21_add_simple(value_1, value_2, &return_value_int, result);
    s21_set_scale(result, scale);
  } else if ((!s21_is_positive(value_1)) & s21_is_positive(value_2)) {
    scale = s21_scale_align(&value_1, &value_2, &return_value_int, result);
    s21_invert(value_1, &return_value_int, &value_1);
    s21_add_simple(value_2, value_1, &return_value_int, result);
    s21_set_scale(result, scale);
    if (is_greater) {
      s21_invert(*result, &return_value_int, result);
      s21_set_scale(result, scale);
      s21_set_sign(result);
    }
  } else if (s21_is_positive(value_1) & (!s21_is_positive(value_2))) {
    scale = s21_scale_align(&value_1, &value_2, &return_value_int, result);
    s21_invert(value_2, &return_value_int, &value_2);
    s21_add_simple(value_1, value_2, &return_value_int, result);
    s21_set_scale(result, scale);
    if (!is_greater) {
      s21_invert(*result, &return_value_int, result);
      s21_set_scale(result, scale);
      s21_set_sign(result);
    }
  } else {
    scale = s21_scale_align(&value_1, &value_2, &return_value_int, result);
    s21_add_simple(value_1, value_2, &return_value_int, result);
    s21_set_sign(result);
    s21_set_scale(result, scale);
  }
  if (return_value == 1) return_value = s21_INF;
  return return_value;
}

s21_decimal s21_mul_simple(s21_decimal x) {
  s21_decimal result = {
      {0, 0, 0, 0},
  };
  int ten = 10;
  x.bits[3] = 0;
  while (ten != 0) {
    int mask = ten & 1;
    if (mask == 1) {
      int temp = 0;
      for (int i = 0; i < 4; i++) {
        unsigned int res = 0;
        for (int k = 32, shift = 1; k > 0; k--, shift <<= 1) {
          int x_sh = result.bits[i] & shift;
          int y_sh = x.bits[i] & shift;
          if ((!x_sh) && y_sh && (!temp)) {
            res = res | shift;
          } else if (x_sh && (!y_sh) && (!temp)) {
            res = res | shift;
          } else if (x_sh && y_sh && (!temp)) {
            temp++;
          } else if ((!x_sh) && (!y_sh) && temp) {
            res = res | shift;
            temp = 0;
          } else if (x_sh && y_sh && temp) {
            res = res | shift;
          }
        }
        result.bits[i] = res;
      }
    }
    ten >>= 1;
    s21_big_shift_left(&x);
  }
  return result;
}

s21_decimal* s21_big_shift_left(s21_decimal* x) {
  for (int i = 3; i >= 0; i--) {
    if (i != 3) {
      unsigned int temp = x->bits[i] & MAX;
      x->bits[i] <<= 1;
      if (temp == MAX) {
        x->bits[i + 1] |= 1;
      }
    } else {
      x->bits[i] <<= 1;
    }
  }
  return x;
}

int is_greater_module(s21_decimal a, s21_decimal b, int* return_value_int,
                      s21_decimal* result) {
  int res = 0;
  s21_scale_align(&a, &b, return_value_int, result);
  for (int i = 95; i >= 0 && !res; i--) {
    if (decbit(a, i) > decbit(b, i))
      res = 1;
    else if ((decbit(a, i)) < (decbit(b, i)))
      break;
  }
  return res;
}

int decbit(s21_decimal a, int position) {
  unsigned int x = position / 32;
  unsigned int y = position % 32;
  int res = 0;
  if (x < 4) res = ((a.bits[x] & ((unsigned)1 << y)) != 0);
  return res;
}
//надо написать функцию которая приводить к одной шкале и к одному знаку
int s21_is_equal(s21_decimal dec1, s21_decimal dec2) {
  int is_equal = -1;

  if (is_equal == -1) {
    if (zero_check(dec1, dec2) == 1) is_equal = 1;
  }

  // if (is_equal == -1) {
  //     int who_is_negative = is_negative(&dec1, &dec2);
  //     if (who_is_negative != 0) is_equal = 0;

  //     check_scale(&dec1, &dec2);
  // }

  for (int i = 95; i >= 0 && is_equal == -1; i--) {
    int bit_dec1 = get_bit(dec1, i);
    int bit_dec2 = get_bit(dec2, i);
    if (bit_dec1 != bit_dec2) is_equal = 0;
  }

  return (is_equal != 0) ? 1 : 0;
}

int zero_check(s21_decimal dec1, s21_decimal dec2) {
  int is_zero = 0;
  s21_decimal* pt1 = &dec1;
  s21_decimal* pt2 = &dec2;

  if (pt1 && pt2) {
    if (!dec1.bits[0] && !dec2.bits[0] && !dec1.bits[1] && !dec2.bits[1] &&
        !dec1.bits[2] && !dec2.bits[2])
      is_zero = 1;
  }
  return is_zero;
}

int get_bit(const s21_decimal decVar, int bit) {
  int res = 0;
  if (bit / 32 < 4) {
    unsigned int mask = 1u << (bit % 32);
    res = decVar.bits[bit / 32] & mask;
  }
  return !!res;
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
  value_1.bits[0] = 4104392954;
  value_2.bits[0] = 4104392954;
  value_1.bits[1] = 1;
  value_2.bits[1] = 1;
  value_1.bits[2] = 2;
  value_2.bits[2] = 2;
  // s21_set_scale(&value_2, 5);
  int return_value = s21_add(value_1, value_2, &value_3);
  printf("return_value == %d\n", return_value);
  printf("value_3.bits[0] == %d\n", value_3.bits[0]);
  printf("value_3.bits[1] == %d\n", value_3.bits[1]);
  printf("value_3.bits[2] == %d\n", value_3.bits[2]);
  printf("value_3.bits[3] == %d\n", value_3.bits[3]);

  s21_decimal check = {{0xcedabe40, 0x99c0c5d, 0x13a3a, 0x80050000}};
  printf("s21_is_equal == %d\n", s21_is_equal(value_3, check));
  for (int i = 95; i >= 0; i--) {
    printf("%d", get_bit(value_3, i));
  }
  printf("\n");
  return 0;
}
