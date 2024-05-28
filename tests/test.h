#ifndef SRC_TESTS_TEST_H
#define SRC_TESTS_TEST_H

#include <stdlib.h>
#include <time.h>

#include "../s21_decimal.h"
#include "check.h"

Suite *convertors_suite(void);
Suite *comparison_suite(void);
Suite *another_suite(void);
Suite *helper_suite(void);
Suite *arithmetic_suite(void);

#endif
