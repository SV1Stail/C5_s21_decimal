#ifndef SRC_S21_DECIMAL_H
#define SRC_S21_DECIMAL_H

#include <math.h>
#include <stdbool.h>

#include "stdio.h"
#include "stdlib.h"

#define MINUS 0x80000000
#define EXP 0x00ff0000
#define MANTISSA 0x003FFFFF

typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef unsigned short int uint16;

#define UINT_MAX 4294967295

typedef union {
  int uin_t;
  float floa_t;
} s21_float_bits;

typedef struct {
  uint32 bits[4];
} s21_decimal;

typedef struct {
  uint64 bits[7];
  uint32 exp;
} s21_big_decimal;

// ---------- Arithmetic Operators ----------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// ---------- Comparison Operators ----------
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);      //+qhoringu
int s21_is_not_equal(s21_decimal, s21_decimal);  //+qhoringu
// ---------- Convertors and parsers ----------
int s21_from_int_to_decimal(int src, s21_decimal *dst);      //+pandanus
int s21_from_float_to_decimal(float src, s21_decimal *dst);  //+corinned
int s21_from_decimal_to_int(s21_decimal src, int *dst);      //+pandanus
int s21_from_decimal_to_float(s21_decimal src, float *dst);  //+corinned
// ---------- Another functions ----------
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);  //+corinned

// -----------Helpers------------------
bool check_exp(s21_decimal src);
bool get_bit(unsigned int src, unsigned int index);
bool get_sign(s21_decimal src);
bool is_null(s21_decimal src);
bool is_even(unsigned int num);
bool getoverflow(s21_big_decimal *value);
bool is_overflow(s21_big_decimal value);
bool pointleft(s21_big_decimal *value);
bool normalize(s21_big_decimal *value);
bool scale_normalize(s21_big_decimal *value_1, s21_big_decimal *value_2);
bool add(s21_big_decimal value_1, s21_big_decimal value_2, s21_decimal *result);
void sub(s21_big_decimal value_1, s21_big_decimal value_2, s21_decimal *result);
void set_1_bit(unsigned int *src, unsigned int index);
void negate_bit(unsigned int *src, unsigned int index);
void set_exp(s21_decimal *value, unsigned int exp);
void set_sign(s21_decimal *src, bool sign);
void zero_normalize(s21_big_decimal *value);
uint32 get_exp(s21_decimal src);
uint32 get_index_first_bit_1(unsigned int mantissa);
uint32 pointright(s21_big_decimal *value);
s21_decimal big_to_norm(s21_big_decimal src);
s21_big_decimal norm_to_big(s21_decimal src);
bool mul(s21_big_decimal value_1, s21_big_decimal value_2, s21_decimal *result);
int get_last_fractional_part(s21_decimal src);
int find_bit(s21_decimal src, int index);
int get_scale(s21_decimal dst);

#endif
