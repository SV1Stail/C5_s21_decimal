
#include "s21_decimal.h"

// Convert an integer to a s21_decimal
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  *dst = (s21_decimal){0};  // Initialize the s21_decimal as 0

  if (src < 0) {
    set_1_bit(&dst->bits[3],
              31);  // Set the sign bit if the source integer is negative
    src = -src;     // Make the source integer positive
  }
  dst->bits[0] =
      src;  // Store the absolute value of the integer in the s21_decimal

  return 0;  // Return 0 to indicate success
}

// Convert a s21_decimal to an integer
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int ret = 1;           // Initialize ret to 1 (failure)
  if (*dst == 0) {       // Check if the destination integer is 0
    *dst = src.bits[0];  // Assign the value from the s21_decimal to the
                         // destination integer
    *dst = *dst * (get_bit(src.bits[3], 31)
                       ? -1
                       : 1);  // Apply the sign based on the sign bit
    ret = 0;                  // Set ret to 0 to indicate success
  }
  return ret;  // Return 0 for success, 1 for failure
}

// Convert a float to a s21_decimal
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int ret = 1;  // Initialize ret to 1 (failure)
  dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] =
      0;  // Initialize s21_decimal as 0

  if (!isinf(src) &&
      !isnan(src)) {  // Check if the source float is not infinity or NaN
    if (src != 0) {   // Check if the source float is not zero
      int sign = *(int *)&src >> 31;  // Extract the sign bit
      int exponent =
          ((*(int *)&src & ~0x80000000) >> 23) - 127;  // Extract the exponent
      double temp =
          (double)fabs(src);  // Calculate the absolute value of the float
      int off = 0;

      for (; off < 28 && (int)temp / (int)pow(2, 21) == 0; temp *= 10, off++) {
      }  // Normalize the float and update 'off'

      temp = round(temp);  // Round the float

      if (off <= 28 && (exponent > -94 && exponent < 96)) {
        s21_float_bits mantissa = {0};
        temp = (float)temp;  // Convert the value to float

        for (; fmod(temp, 10) == 0 && off > 0; off--, temp /= 10) {
        }  // Remove trailing zeros and update 'off'

        mantissa.floa_t = temp;  // Store the float value in a bit structure
        exponent = ((*(int *)&mantissa.floa_t & ~0x80000000) >> 23) -
                   127;  // Extract the exponent
        dst->bits[exponent / 32] |=
            1 << exponent % 32;  // Set the corresponding bit in the s21_decimal

        for (int i = exponent - 1, j = 22; j >= 0; i--, j--) {
          if ((mantissa.uin_t & (1 << j)) != 0) {
            dst->bits[i / 32] |=
                1 << i % 32;  // Set the mantissa bits in the s21_decimal
          }
        }

        dst->bits[3] =
            (sign << 31) |
            (off << 16);  // Set the sign and scale bits in the s21_decimal
      }
    }
    ret = 0;  // Set ret to 0 to indicate success
  }
  return ret;  // Return 0 for success, 1 for failure
}

// Convert a s21_decimal to a float
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double temp = (double)*dst;
  for (int i = 0; i < 96; i++) {
    temp += find_bit(src, i) * pow(2, i);
  }  // Convert the mantissa bits to a float value

  temp = temp * pow(10, -get_scale(src));      // Adjust for the scale
  if (find_bit(src, 127)) temp = temp * (-1);  // Apply the sign
  *dst = temp;  // Store the float value in the destination
  return 0;     // Return 0 to indicate success
}

//

// Function to get the scale of a s21_decimal number
int get_scale(s21_decimal dst) {
  int mask = 127 << 16;                    // Create a mask to extract the scale
  int scale = (mask & dst.bits[3]) >> 16;  // Extract the scale from the bits
  return scale;
}

// Function to find a specific bit in a s21_decimal number
int find_bit(s21_decimal dst, int index) {
  int mask = 1u << (index %
                    32);  // Create a mask to extract the bit at the given index
  return (dst.bits[index / 32] & mask) != 0;  // Check if the bit is set
}

// If the exponent in a decimal is greater than 28, return 1, otherwise return 0
bool check_exp(s21_decimal src) {
  if (get_exp(src) > 28) return 1;
  return 0;
}

// Get a bit from an unsigned integer at the specified index (index ranging from
// 0 to 31)
bool get_bit(unsigned int src, unsigned int index) {
  return (src >> index) &
         1;  // Extract and return the bit at the specified index
}

// Set a bit to 1 at the specified index in an unsigned integer
void set_1_bit(unsigned int *src, unsigned int index) {
  *src |= (1u << index);  // Set the bit at the specified index to 1
}

// Set a bit to 0 at the specified index in an unsigned integer
void set_0_bit(unsigned int *src, unsigned int index) {
  *src &= ~(1u << index);  // Set the bit at the specified index to 0
}

// Function to negate a bit at the specified index in an unsigned integer
void negate_bit(unsigned int *src, unsigned int index) {
  *src ^= (1u << index);  // Negate (flip) the bit at the specified index
}

// Check the sign of a s21_decimal number
bool get_sign(s21_decimal src) {
  if (get_bit(src.bits[3], 31)) {
    return 1;  // If the sign bit is set, return 1 (negative)
  }
  return 0;  // Otherwise, return 0 (positive)
}

// Set the sign of a s21_decimal number (1 for negative, 0 for positive)
void set_sign(s21_decimal *src, bool sign) {
  src->bits[3] |=
      sign ? MINUS : 0;  // Set the sign bit based on the provided boolean value
}

// Check if the mantissa of a s21_decimal is zero
bool is_null(s21_decimal src) {
  if (src.bits[0] == 0 && src.bits[1] == 0 && src.bits[2] == 0)
    return 1;  // If all mantissa bits are zero, return 1
  return 0;    // Otherwise, return 0
}

// Get the exponent value from a s21_decimal in decimal form
unsigned int get_exp(s21_decimal src) {
  return (src.bits[3] & EXP) >>
         16;  // Extract and return the exponent value in decimal form
}

// Set the exponent value in a s21_decimal from a decimal representation
void set_exp(s21_decimal *value, unsigned int exp) {
  value->bits[3] |= (exp << 16);  // Set the exponent value in the s21_decimal
}

// Add the exponent of a s21_decimal with the exponent of an argument and update
// it in dest. Return 1 if the resulting sum is greater than 28. (Note: This
// function is commented out in the provided code) bool add_exp(s21_decimal src,
// s21_decimal *dest, unsigned int exp) {
//   unsigned int new_exp = get_exp(src) + exp;
//   if (new_exp > 28) return 1;
//   set_exp(dest, new_exp);
//   return 0;
// }

// Check if a number is even or odd
bool is_even(unsigned int num) {
  return get_bit(num, 0)
             ? 0
             : 1;  // Check if the least significant bit is 0 (even) or 1 (odd)
}

// Get the index of the most significant bit set to 1 in the mantissa
unsigned int get_index_first_bit_1(unsigned int mantissa) {
  for (unsigned int i = 31; i > 0; i--) {
    unsigned int bit = get_bit(mantissa, i);
    if (bit)
      return i;  // Find and return the index of the first set bit from the left
  }
  return 0;  // If no set bit is found, return 0
}

// Convert a big decimal to a normal (small) decimal
s21_decimal big_to_norm(s21_big_decimal src) {
  s21_decimal res = {0};
  for (int i = 0; i < 3; i++)
    res.bits[i] = src.bits[i] & UINT_MAX;  // Copy and truncate the bits
  set_exp(&res, (uint32)src.exp);          // Set the exponent
  return res;
}

// Convert a normal (small) decimal to a big decimal
s21_big_decimal norm_to_big(s21_decimal src) {
  s21_big_decimal res = {0};
  unsigned int exp = get_exp(src);
  for (int i = 0; i < 3; i++)
    res.bits[i] = src.bits[i] & UINT_MAX;  // Copy and truncate the bits
  res.exp = exp;                           // Set the exponent
  return res;
}

// Function to check for overflow
bool getoverflow(s21_big_decimal *value) {
  uint32 overflow = 0;
  for (int i = 0; i < 7; i++) {
    value->bits[i] += overflow;  // Add the overflow from the previous iteration
    overflow = value->bits[i] >> 32;  // Calculate the new overflow
    value->bits[i] &= UINT_MAX;       // Truncate the bits
  }
  if (is_overflow(*value)) overflow = 1;  // Check for overall overflow
  return overflow;  // Return 1 if there is overflow, otherwise 0
}

// Function to shift left (multiply by 10) with potential overflow check
bool pointleft(s21_big_decimal *value) {
  s21_big_decimal copy = *value;
  bool ret = 0;
  for (int i = 0; i < 7; i++) {
    copy.bits[i] *= 10;  // Multiply by 10
  }
  copy.exp++;                // Increment the exponent
  ret = getoverflow(&copy);  // Check for overflow
  if (!ret && value->exp < 28)
    *value =
        copy;  // Update the value if no overflow and exponent is within limits
  return ret;  // Return 1 if there is overflow, otherwise 0
}

// Function to shift right (divide by 10) with potential remainder
uint32 pointright(s21_big_decimal *value) {
  uint16 remainder = 0;
  if (get_exp(big_to_norm(*value)) > 0) {
    s21_big_decimal copy = *value;
    for (int i = 6; i >= 0; i--) {
      copy.bits[i] += (uint64)remainder
                      << 32;  // Add the remainder from the previous iteration
      remainder = copy.bits[i] % 10;  // Calculate the new remainder
      copy.bits[i] /= 10;             // Divide by 10
    }
    copy.exp--;     // Decrement the exponent
    *value = copy;  // Update the value
  }
  return remainder;  // Return the final remainder
}

// Check for overflow in a big decimal
bool is_overflow(s21_big_decimal value) {
  if (value.bits[3] || value.bits[4] || value.bits[5] || value.bits[6]) {
    return 1;  // If any bits in the upper portion are set, return 1 (overflow)
  }
  return 0;  // Otherwise, return 0
}

// Function for banker's rounding
uint32 bank_round(uint32 value, uint16 remainders[], uint16 index) {
  if (remainders[index] > 5) {
    return value + 1;  // If the remainder is greater than 5, round up
  } else if (remainders[index] < 5) {
    return value;  // If the remainder is less than 5, round down
  } else {
    while (index > 0) {
      if (remainders[--index])
        return value + 1;  // Round up if there is a non-zero remainder
    }
    if (is_even(value))
      return value;  // Round down if the value is even
    else
      return value + 1;  // Round up if the value is odd
  }
}

// Function for normalization. Continue normalizing until there are no non-zero
// bits to the right
bool normalize(s21_big_decimal *value) {
  s21_big_decimal copy = *value;
  uint16 remainders[28] = {0};
  uint16 i = 0;
  while (copy.exp > 0 && is_overflow(copy)) {
    remainders[i++] = pointright(&copy);  // Normalize by shifting right
  }
  if (copy.exp == 0 && is_overflow(copy))
    return 1;  // If not fully normalized, return 1
  copy.bits[0] =
      bank_round(copy.bits[0], remainders, i);  // Perform banker's rounding
  *value = copy;                                // Update the value
  return 0;                                     // Return 0 if fully normalized
}

// Function to scale and normalize two big decimals to the same exponent format
bool scale_normalize(s21_big_decimal *value_1, s21_big_decimal *value_2) {
  bool ret = 0;
  if (value_1->exp == value_2->exp)
    return ret;  // If exponents are already the same, return 0
  if (value_1->exp > value_2->exp) {
    while (value_2->exp < value_1->exp) {
      ret = pointleft(value_2);  // Scale and normalize value_2 to match value_1
    }
  } else {
    while (value_1->exp < value_2->exp) {
      ret = pointleft(value_1);  // Scale and normalize value_1 to match value_2
    }
  }
  return ret;  // Return 1 if there is overflow during normalization
}

// Normalize a big decimal by removing trailing zeros
void zero_normalize(s21_big_decimal *value) {
  while (value->exp > 0) {
    s21_big_decimal copy = *value;
    unsigned int rem = pointright(&copy);
    if (rem == 0)
      *value = copy;  // If the remainder is zero, update the value and continue
    else
      break;  // If the remainder is non-zero, stop normalizing
  }
}

// Function to add two big decimals and store the result in a small decimal
bool add(s21_big_decimal value_1, s21_big_decimal value_2,
         s21_decimal *result) {
  int ret = 0;
  s21_big_decimal copy = value_1;

  for (int i = 0; i < 3; i++) {
    copy.bits[i] += value_2.bits[i];  // Add the corresponding bits
  }

  ret = getoverflow(&copy);         // Check for overflow
  if (ret) ret = normalize(&copy);  // Normalize if there is overflow
  *result = big_to_norm(copy);      // Convert the result to a small decimal

  return ret;  // Return 1 if there is overflow, otherwise 0
}

// Function to subtract two big decimals and store the result in a small decimal
void sub(s21_big_decimal value_1, s21_big_decimal value_2,
         s21_decimal *result) {
  s21_big_decimal copy = value_1;
  bool carry = 0;

  for (int i = 0; i < 3; i++) {
    copy.bits[i] +=
        carry ? ~(value_2.bits[i] + 1) + 1
              : ~value_2.bits[i] + 1;  // Perform two's complement subtraction
    carry = 0;
    if (value_1.bits[i] < value_2.bits[i]) carry = 1;  // Set carry if necessary
  }

  *result = big_to_norm(copy);  // Convert the result to a small decimal
}

// Function to get the last fractional part of a small decimal
int get_last_fractional_part(s21_decimal src) {
  s21_big_decimal big_src = norm_to_big(src);
  int result = 0;
  while (get_exp(big_to_norm(big_src))) {
    result = pointright(&big_src);  // Extract the last fractional part
  }
  return result;
}

// Function to multiply two big decimals and store the result in a small decimal
bool mul(s21_big_decimal value_1, s21_big_decimal value_2,
         s21_decimal *result) {
  s21_big_decimal temp = {0};

  // Perform multiplication and handle potential overflow
  temp.bits[0] = value_1.bits[0] * value_2.bits[0];
  getoverflow(&temp);
  temp.bits[1] += value_1.bits[1] * value_2.bits[0];
  getoverflow(&temp);
  temp.bits[1] += value_1.bits[0] * value_2.bits[1];
  getoverflow(&temp);
  temp.bits[2] += value_1.bits[0] * value_2.bits[2];
  getoverflow(&temp);
  temp.bits[2] += value_1.bits[1] * value_2.bits[1];
  getoverflow(&temp);
  temp.bits[2] += value_1.bits[2] * value_2.bits[0];
  getoverflow(&temp);
  temp.bits[3] += value_1.bits[1] * value_2.bits[2];
  getoverflow(&temp);
  temp.bits[3] += value_1.bits[2] * value_2.bits[1];
  getoverflow(&temp);
  temp.bits[4] += value_1.bits[2] * value_2.bits[2];
  getoverflow(&temp);

  temp.exp = value_1.exp + value_2.exp;
  zero_normalize(&temp);  // Normalize the result

  if (temp.bits[3] || temp.exp > 28) return 1;  // Check for overflow
  *result = big_to_norm(temp);  // Convert the result to a small decimal
  return 0;                     // Return 1 if there is overflow, otherwise 0
}
