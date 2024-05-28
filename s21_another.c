#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  negate_bit(&result->bits[3], 31);
  return 0;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int fraction_part = get_last_fractional_part(value);
  s21_truncate(value, result);
  if (fraction_part > 4) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_add(*result, one, result);
  }

  return 0;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_big_decimal tmp = norm_to_big(value);
  bool sign = get_sign(value);
  while (get_exp(big_to_norm(tmp))) {
    pointright(&tmp);
  }
  *result = big_to_norm(tmp);
  set_sign(result, sign);
  return 0;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  bool sign = get_sign(value);
  s21_truncate(value, result);
  if (sign) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_sub(*result, one, result);
  }
  return 0;
}
