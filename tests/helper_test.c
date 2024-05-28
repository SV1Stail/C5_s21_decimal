#include "test.h"

// Проверка маски минуса
START_TEST(minus_test) {
  unsigned int n = 1 << 31;
  ck_assert_uint_eq(n, MINUS);
}
END_TEST
// Проверка функции получения бита по индексу
START_TEST(get_bit_test_1) {
  unsigned int bit = 1;
  unsigned int n = bit << 31;
  bool res = get_bit(n, 31);
  ck_assert_uint_eq(res, bit);
}
END_TEST

START_TEST(get_bit_test_2) {
  unsigned int bit = 1;
  unsigned int n = bit << 0;
  unsigned int res = get_bit(n, 0);
  ck_assert_uint_eq(res, bit);
}
END_TEST

START_TEST(get_exp_test) {
  unsigned int n = 5;
  unsigned int exp1 = n << 16;
  s21_decimal src = {{0, 0, 0, exp1}};
  unsigned int exp2 = get_exp(src);
  ck_assert_uint_eq(exp2, n);
}
END_TEST
START_TEST(negate_bit_test) {
  unsigned int n = MINUS;
  negate_bit(&n, 31);
  ck_assert_uint_eq(n, 0);
  negate_bit(&n, 31);
  ck_assert_uint_eq(n, MINUS);
}
END_TEST
// Проверка функции установки экспаненты в децимал
START_TEST(set_exp_test_1) {
  unsigned int exp1 = 16;
  s21_decimal src = {0};
  set_exp(&src, exp1);
  unsigned int exp2 = get_exp(src);
  ck_assert_uint_eq(exp2, exp1);
}
END_TEST

START_TEST(set_exp_test_2) {
  unsigned int exp1 = 16;
  s21_decimal src = {{0, 0, 0, MINUS}};
  set_exp(&src, exp1);
  int sign = get_sign(src);
  unsigned int exp2 = get_exp(src);
  ck_assert_uint_eq(exp2, exp1);
  ck_assert_uint_eq(sign, 1);
}
END_TEST
// Проверка функции валидации экспаненты что она меньше 28
START_TEST(check_exp_test_1) {
  s21_decimal src = {{0, 0, 0, 0}};
  set_exp(&src, 16);
  int ret = check_exp(src);
  ck_assert_uint_eq(ret, 0);
}
END_TEST

START_TEST(check_exp_test_2) {
  s21_decimal src = {{0, 0, 0, 0}};
  set_exp(&src, 29);
  int ret = check_exp(src);
  ck_assert_uint_eq(ret, 1);
}
END_TEST
// Тест функции проверки на четность/нечетность
START_TEST(is_even_test_1) {
  unsigned int n = 5;
  int ret = is_even(n);
  ck_assert_uint_eq(ret, 0);
}
END_TEST

START_TEST(is_even_test_2) {
  unsigned int n = 6;
  int ret = is_even(n);
  ck_assert_uint_eq(ret, 1);
}
END_TEST
// Тест для функции получения индекса первого значимого бита
START_TEST(get_index_first_bit_1_test_1) {
  unsigned int mantissa = 1;
  unsigned int index = get_index_first_bit_1(mantissa);
  unsigned int res = 0;
  ck_assert_uint_eq(index, res);
}
END_TEST

START_TEST(get_index_first_bit_1_test_2) {
  unsigned int mantissa = UINT_MAX;
  unsigned int index = get_index_first_bit_1(mantissa);
  unsigned int res = 31;
  ck_assert_uint_eq(index, res);
}
END_TEST

START_TEST(get_index_first_bit_1_test_3) {
  unsigned int n = 15;
  unsigned int mantissa = 1 << n;
  unsigned int index = get_index_first_bit_1(mantissa);
  ck_assert_uint_eq(index, n);
}
END_TEST

START_TEST(get_index_first_bit_1_test_4) {
  unsigned int n = 15;
  unsigned int mantissa = 1 << n;
  unsigned int index = get_index_first_bit_1(mantissa);
  unsigned int res = mantissa >> index;
  ck_assert_uint_eq(res, 1);
  ck_assert_uint_eq(index, n);
}
END_TEST

START_TEST(big_to_norm_test_1) {
  s21_big_decimal src = {{12, 13, 45, 0, 0, 0, 0}, 14};
  s21_decimal res = big_to_norm(src);
  ck_assert_uint_eq(res.bits[0], 12);
  ck_assert_uint_eq(res.bits[1], 13);
  ck_assert_uint_eq(res.bits[2], 45);
  ck_assert_uint_eq(res.bits[3], 14 << 16);
}
END_TEST

START_TEST(norm_to_big_test_1) {
  s21_decimal src = {{12, 13, 45, 12 << 16}};
  s21_big_decimal res = norm_to_big(src);
  ck_assert_uint_eq(res.bits[0], 12);
  ck_assert_uint_eq(res.bits[1], 13);
  ck_assert_uint_eq(res.bits[2], 45);
  ck_assert_uint_eq(res.exp, 12);
}
END_TEST

START_TEST(getoverflow_test_1) {
  s21_big_decimal src = {0};
  src.bits[0] = (uint64)UINT_MAX + 1;
  bool ret = getoverflow(&src);
  ck_assert_uint_eq(src.bits[0], 0);
  ck_assert_uint_eq(src.bits[1], 1);
  ck_assert_uint_eq(ret, 0);
}
END_TEST

START_TEST(getoverflow_test_2) {
  s21_big_decimal src = {0};
  src.bits[0] = UINT64_MAX;
  bool ret = getoverflow(&src);
  ck_assert_uint_eq(src.bits[0], UINT_MAX);
  ck_assert_uint_eq(src.bits[1], UINT_MAX);
  ck_assert_uint_eq(ret, 0);
}
END_TEST

START_TEST(getoverflow_test_3) {
  s21_big_decimal src = {0};
  src.bits[6] = UINT64_MAX;
  bool ret = getoverflow(&src);
  ck_assert_uint_eq(ret, 1);
}
END_TEST

START_TEST(pointleft_test_1) {
  s21_big_decimal src = {0};
  src.bits[0] = 1;
  src.bits[1] = UINT_MAX;
  bool ret = pointleft(&src);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(src.bits[0], 10);
  ck_assert_uint_eq(src.bits[1], 4294967286);
  ck_assert_uint_eq(src.bits[2], 9);
  ck_assert_uint_eq(src.exp, 1);
}
END_TEST

START_TEST(pointleft_test_2) {
  s21_big_decimal src = {0};
  src.bits[6] = UINT64_MAX;
  bool ret = pointleft(&src);
  ck_assert_uint_eq(ret, 1);
  ck_assert_uint_eq(src.bits[6], UINT64_MAX);
}
END_TEST

START_TEST(pointleft_test_3) {
  s21_big_decimal src = {0};
  src.bits[0] = 11;
  bool ret = pointleft(&src);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(src.bits[0], 110);
}
END_TEST

START_TEST(pointright_test_1) {
  s21_big_decimal src = {{0}, 1};
  src.bits[0] = 10;
  uint32 ret = pointright(&src);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(src.bits[0], 1);
  ck_assert_uint_eq(src.exp, 0);
}
END_TEST

START_TEST(pointright_test_2) {
  s21_big_decimal src = {{0}, 1};
  src.bits[0] = 1234;
  uint32 ret = pointright(&src);
  ck_assert_uint_eq(ret, 4);
  ck_assert_uint_eq(src.bits[0], 123);
  ck_assert_uint_eq(src.exp, 0);
}
END_TEST

START_TEST(is_overflow_test_1) {
  s21_big_decimal src = {{0}, 10};
  src.bits[2] = 123456789;
  src.bits[3] = 123456789;
  bool ret = is_overflow(src);
  ck_assert_uint_eq(ret, 1);
}
END_TEST

START_TEST(is_overflow_test_2) {
  s21_big_decimal src = {{0}, 10};
  src.bits[2] = 123456789;
  bool ret = is_overflow(src);
  ck_assert_uint_eq(ret, 0);
}
END_TEST

START_TEST(is_overflow_test_3) {
  s21_big_decimal src = {{0}, 10};
  src.bits[3] = 1;
  bool ret1 = is_overflow(src);
  uint32 reminder = pointright(&src);
  bool ret2 = is_overflow(src);
  ck_assert_uint_eq(ret1, 1);
  ck_assert_uint_eq(reminder, 6);
  ck_assert_uint_eq(ret2, 0);
}
END_TEST

START_TEST(normalize_test_1) {
  s21_big_decimal src = {{0}, 10};
  src.bits[3] = 123456789;
  bool ret = normalize(&src);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(src.bits[3], 0);
  ck_assert_uint_eq(src.exp, 1);
}
END_TEST

START_TEST(normalize_test_2) {
  s21_big_decimal src = {{0}, 10};
  src.bits[3] = 12;
  bool ret = normalize(&src);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(src.bits[3], 0);
  ck_assert_uint_eq(src.bits[0], 3951369912);
  ck_assert_uint_eq(src.exp, 8);
}
END_TEST

START_TEST(scale_normalize_test_1) {
  s21_big_decimal value_1 = {{0}, 3};
  s21_big_decimal value_2 = {{0}, 1};
  value_1.bits[0] = 12345;
  value_2.bits[0] = 12345;
  bool ret = scale_normalize(&value_1, &value_2);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(value_1.bits[0], 12345);
  ck_assert_uint_eq(value_2.bits[0], 1234500);
  ck_assert_uint_eq(value_1.exp, 3);
  ck_assert_uint_eq(value_1.exp, 3);
}
END_TEST

START_TEST(add_test_1) {
  s21_big_decimal value_1 = {{0}, 1};
  s21_big_decimal value_2 = {{0}, 1};
  s21_decimal result = {0};
  value_1.bits[0] = 1;
  value_2.bits[0] = 1;
  bool ret = add(value_1, value_2, &result);
  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(value_1.bits[0], 1);
  ck_assert_uint_eq(value_2.bits[0], 1);
}
END_TEST

START_TEST(sub_test_1) {
  s21_big_decimal value_1 = {{0}, 0};
  s21_big_decimal value_2 = {{0}, 0};
  s21_decimal result = {0};
  value_1.bits[0] = 4;
  value_2.bits[0] = 2;
  sub(value_1, value_2, &result);
  ck_assert_uint_eq(result.bits[0], 2);
}
END_TEST

START_TEST(sub_test_2) {
  s21_big_decimal value_1 = {{0}, 0};
  s21_big_decimal value_2 = {{0}, 0};
  s21_decimal result = {0};
  value_1.bits[0] = 78327;
  value_2.bits[0] = 695;
  sub(value_1, value_2, &result);
  ck_assert_uint_eq(result.bits[0], 77632);
}
END_TEST

START_TEST(sub_test_3) {
  s21_big_decimal value_1 = {{0}, 0};
  s21_big_decimal value_2 = {{0}, 0};
  s21_decimal result = {0};
  value_1.bits[1] = 1;
  value_1.bits[0] = 695;
  value_2.bits[0] = 783745;
  sub(value_1, value_2, &result);
  ck_assert_uint_eq(result.bits[0], 4294184246);
  ck_assert_uint_eq(result.bits[1], 0);
}
END_TEST

Suite *helper_suite(void) {
  Suite *s;
  TCase *tc_helper;

  s = suite_create("Helper");

  tc_helper = tcase_create("Helper functions");

  // ---------- Helper functions ----------
  tcase_add_test(tc_helper, minus_test);
  tcase_add_test(tc_helper, get_bit_test_1);
  tcase_add_test(tc_helper, get_bit_test_2);
  tcase_add_test(tc_helper, get_exp_test);
  tcase_add_test(tc_helper, set_exp_test_1);
  tcase_add_test(tc_helper, set_exp_test_2);
  tcase_add_test(tc_helper, check_exp_test_1);
  tcase_add_test(tc_helper, check_exp_test_2);
  tcase_add_test(tc_helper, is_even_test_1);
  tcase_add_test(tc_helper, is_even_test_2);
  tcase_add_test(tc_helper, get_index_first_bit_1_test_1);
  tcase_add_test(tc_helper, get_index_first_bit_1_test_2);
  tcase_add_test(tc_helper, get_index_first_bit_1_test_3);
  tcase_add_test(tc_helper, get_index_first_bit_1_test_4);
  tcase_add_test(tc_helper, big_to_norm_test_1);
  tcase_add_test(tc_helper, norm_to_big_test_1);
  tcase_add_test(tc_helper, getoverflow_test_1);
  tcase_add_test(tc_helper, getoverflow_test_2);
  tcase_add_test(tc_helper, getoverflow_test_3);
  tcase_add_test(tc_helper, pointleft_test_1);
  tcase_add_test(tc_helper, pointleft_test_2);
  tcase_add_test(tc_helper, pointleft_test_3);
  tcase_add_test(tc_helper, pointright_test_1);
  tcase_add_test(tc_helper, pointright_test_2);
  tcase_add_test(tc_helper, is_overflow_test_1);
  tcase_add_test(tc_helper, is_overflow_test_2);
  tcase_add_test(tc_helper, is_overflow_test_3);
  tcase_add_test(tc_helper, normalize_test_1);
  tcase_add_test(tc_helper, normalize_test_2);
  tcase_add_test(tc_helper, negate_bit_test);
  tcase_add_test(tc_helper, scale_normalize_test_1);
  tcase_add_test(tc_helper, add_test_1);
  tcase_add_test(tc_helper, sub_test_1);
  tcase_add_test(tc_helper, sub_test_2);
  tcase_add_test(tc_helper, sub_test_3);

  suite_add_tcase(s, tc_helper);

  return s;
}
