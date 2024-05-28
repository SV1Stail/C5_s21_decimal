#include "s21_decimal.h"
/*
      ARITHMETIC
      FUNCTIONS
*/
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;

  bool check_1 = is_null(value_1);
  bool check_2 = is_null(value_2);

  bool check_exp_1 = check_exp(value_1);
  bool check_exp_2 = check_exp(value_1);

  if (check_exp_1 || check_exp_2) {
    return 2;
  }

  if (check_1 && check_2) {
    *result = value_1;
    result->bits[3] = 0;
    return ret;
  }
  if (check_1 || check_2) {
    *result = check_1 == 1 ? value_2 : value_1;
    return ret;
  }
  s21_big_decimal copy_1 = {0};
  s21_big_decimal copy_2 = {0};

  bool sign_1 = get_sign(value_1);
  bool sign_2 = get_sign(value_2);

  copy_1 = norm_to_big(value_1);
  copy_2 = norm_to_big(value_2);

  ret = scale_normalize(&copy_1, &copy_2);

  if (sign_1 == sign_2) {
    ret = add(copy_1, copy_2, result);
  } else {
    s21_decimal tmp_1 = {0};
    s21_decimal tmp_2 = {0};
    if (sign_1 == 1) {
      s21_negate(value_1, &tmp_1);
      tmp_2 = value_2;
    } else {
      s21_negate(value_2, &tmp_2);
      tmp_1 = value_1;
    }
    if (s21_is_less(tmp_1, tmp_2)) {
      sub(copy_2, copy_1, result);
      sign_1 = 0;
    } else {
      sub(copy_1, copy_2, result);
      sign_2 = 0;
    }
  }

  bool sign = sign_1 | sign_2;

  set_sign(result, sign);

  return ret;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  s21_decimal copy = {0};

  bool sign_1 = get_sign(value_1);
  bool sign_2 = get_sign(value_2);

  if (sign_1 == sign_2) {
    s21_negate(value_2, &copy);
    ret = s21_add(value_1, copy, result);
  } else {
    s21_decimal tmp_1 = {0};
    s21_decimal tmp_2 = {0};
    if (sign_1 == 1) {
      s21_negate(value_1, &tmp_1);
      tmp_2 = value_2;
    } else {
      s21_negate(value_2, &tmp_2);
      tmp_1 = value_1;
    }
    ret = s21_add(tmp_1, tmp_2, result);
    set_sign(result, sign_1);
  }

  return ret ? 2 : 0;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  *result = (s21_decimal){0};

  bool check_1 = is_null(value_1);
  bool check_2 = is_null(value_2);

  if (check_1 || check_2) return 0;

  bool sign_1 = get_sign(value_1);
  bool sign_2 = get_sign(value_2);

  s21_big_decimal copy_1 = norm_to_big(value_1);
  s21_big_decimal copy_2 = norm_to_big(value_2);

  ret = mul(copy_1, copy_2, result);

  bool sign = sign_1 ^ sign_2;
  set_sign(result, sign);

  return ret;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  *result = (s21_decimal){0};

  bool check_1 = is_null(value_1);
  bool check_2 = is_null(value_2);

  if (check_1) return 0;

  if (check_2) return 3;

  return ret;
}

/*
      COMPARSION
      FUNCTIONS
*/

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int isEqual = 1;
  int check = 0;
  for (int i = 0; i < 3 && isEqual == 1; i++) {  // проверяем мантиссу на 0
    if ((value1.bits[i] != 0) && (value2.bits[i] != 0)) {
      isEqual = 0;
    }
  }
  // проверка на знак
  if (isEqual == 0 && check == 0) {
    if (get_sign(value1) != get_sign(value2)) check = 1;
  }
  //если мантисса не ноль (при любом знаке и при любой экспоненте), то переходим
  //к нормализации
  if (isEqual == 0 && check == 0) {
    s21_big_decimal vb1 = norm_to_big(value1);
    s21_big_decimal vb2 = norm_to_big(value2);
    scale_normalize(&vb1, &vb2);
    int test = 4;
    for (int i = 3; i >= 0 && test == i + 1; i--) {
      if (vb1.bits[i] == vb2.bits[i]) {
        test--;
      }
    }
    if (test == 0) isEqual = 1;
  }
  return isEqual;
}

int s21_is_not_equal(s21_decimal value1, s21_decimal value2) {
  return !(s21_is_equal(value1, value2));
}

int s21_is_less(s21_decimal value1, s21_decimal value2) {
  int less = 1;
  if (s21_is_equal(value1, value2) ==
      1) {  //если равно, то не меньше (неправильно возвращаем 0)
    less = 0;
  } else if (get_sign(value1) == 0 &&
             get_sign(value2) == 1) {  //левое положительное, правое
                                       //отрицательное, тоже неверно (0)
    less = 0;
  } else if (get_sign(value1) == 1 &&
             get_sign(value2) == 0) {  //левое отрицательное:, правое
                                       //положительное, все верно (1)
    less = 1;
  } else {  //не попали ни в один случай, нормализуем и сравниваем по мантиссе
    int sign = get_sign(value1);
    s21_big_decimal vb1 = norm_to_big(value1);
    s21_big_decimal vb2 = norm_to_big(value2);
    scale_normalize(&vb1, &vb2);
    int test = 0;
    for (int i = 2; sign == 1 && i >= 0 && test == 0; i--) {
      if (vb1.bits[i] > vb2.bits[i])
        test = 1;  //больше по мантиссе при отрицательном знаке (возвращаем 1)
      else if (vb1.bits[i] < vb2.bits[i])
        test = 2;  //меньше по мантиссе при отрицательном знаке (возвращаем 0)
    }
    for (int i = 2; sign == 0 && i >= 0 && test == 0; i--) {
      if (vb1.bits[i] < vb2.bits[i])
        test = 1;  //меньше по мантиссе при положительном знаке (возвращаем 1)
      else if (vb1.bits[i] > vb2.bits[i])
        test = 2;  //больше по мантиссе при положительном знаке (возвращаем 0)
    }
    if (test == 1)
      less = 1;
    else
      less = 0;
  }
  return less;
}

int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2) {
  int l_or_e = 0;
  if (s21_is_equal(value1, value2) == 1) {  //если равно, то возвращаем 1
    l_or_e = 1;
  } else {  // проверка на меньше
    l_or_e = s21_is_less(value1, value2);
  }
  return l_or_e;
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {
  int greater = 1;
  if (s21_is_equal(value1, value2) ==
      1) {  //если равно, то возвращаем 0 (так как не больше)
    greater = 0;
  } else if (get_sign(value1) == 0 &&
             get_sign(value2) ==
                 1) {  //левое положительное, правое отрицательное, то верно (1)
    greater = 1;
  } else if (get_sign(value1) == 1 &&
             get_sign(value2) ==
                 0) {  //левое отрицательное, правое положительное, неверно (0)
    greater = 0;
  } else {  //не попали ни в один случай, нормализуем и сравниваем по мантиссе
    int sign = get_sign(value1);
    s21_big_decimal vb1 = norm_to_big(value1);
    s21_big_decimal vb2 = norm_to_big(value2);
    scale_normalize(&vb1, &vb2);
    int test = 0;
    for (int i = 2; sign == 1 && i >= 0 && test == 0; i--) {
      if (vb1.bits[i] < vb2.bits[i])
        test = 1;  //меньше по мантиссе при отрицательном знаке (возвращаем 1)
      else if (vb1.bits[i] > vb2.bits[i])
        test = 2;  //больше по мантиссе при отрицательном знаке (возвращаем 0)
    }
    for (int i = 2; sign == 0 && i >= 0 && test == 0; i--) {
      if (vb1.bits[i] > vb2.bits[i])
        test = 1;  //больше по мантиссе при положительном знаке (возвращаем 1)
      else if (vb1.bits[i] < vb2.bits[i])
        test = 2;  //меньше по мантиссе при положительном знаке (возвращаем 0)
    }
    if (test == 1)
      greater = 1;
    else
      greater = 0;
  }
  return greater;
}

int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2) {
  int g_or_e = 1;
  if (s21_is_equal(value1, value2) == 1) {  //если равно, то возвращаем 1
    g_or_e = 1;
  } else {
    g_or_e = s21_is_greater(value1, value2);
  }
  return g_or_e;
}
