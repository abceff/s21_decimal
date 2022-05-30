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

s21_decimal s21_add_simple(s21_decimal value_1, s21_decimal value_2,
                           int* return_value_int) {
  s21_decimal result = {{0, 0, 0, 0}, 0};
  int tmp = 0;  //тут будет 1 если мы складываем 1+1 бит и переносим 1 в
                //следующий разряд
  //проходимся по всем битам всех разрядов каждого числа
  for (int i = 0; i < 96; i++) {
    int bit_value_1 = get_bit(value_1, i);
    int bit_value_2 = get_bit(value_2, i);
    if (!bit_value_1 && !bit_value_2) {  //оба бита равны нулю
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 1
        set_bit(&result, i, 1);
        tmp = 0;
      } else {
        set_bit(&result, i, 0);  //записываем в результат 0, т.к. 0+0=0
      }
    } else if (bit_value_1 && bit_value_2) {  //оба бита 1
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 1
        set_bit(&result, i, 1);
        tmp = 1;  //тоже равен 1, т.к. остается от текущей операции
      } else {
        set_bit(&result, i, 0);
        tmp = 1;
      }
    } else {      //один равен 1
      if (tmp) {  //тут если остался 1 бит после сложения предудущего бита
                  //записываем 0 и переносим 1 (т.к. 1+1 = 10)
        set_bit(&result, i, 0);
        tmp = 1;
      } else {
        set_bit(&result, i, 1);
      }
    }
    if (i == 95 && tmp)
      return_value_int = 1;  //дошли до последнего бита и остается лишний бит
                             //= происхоит переполнение
  }
  return result;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int return_value_int = 0;  //тут я что-то крышой поехала и мне кажется это
                             //по-другому можно реализовать но пока так будет
  //не дает внутри функкции просто инф присваивать инту*
  int return_value = s21_OK;
}

//надо написать функцию которая приводить к одной шкале и к одному знаку