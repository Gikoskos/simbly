#ifndef SIMBLY_COMMON_H__
#define SIMBLY_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <voids.h>


/* Let's set some limits */
#define MAX_INPUT_STR_LEN 1023

#define MAX_INT_ARRAY_LEN 4096

#define MAX_INT_STR_LEN 9

//MAX_ALLOWED_SYMBOL_LEN always has to be less than MAX_INPUT_STR_LEN!!!
#define MAX_ALLOWED_SYMBOL_LEN 127

#define GLOBAL_TABLE_INIT_SIZE 24

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))
#define MAKE_STR(x) #x

#define TERM_RED  "\x1B[31m"
#define TERM_YEL  "\x1B[33m"
#define TERM_MAG  "\x1B[35m"
#define TERM_CYAN  "\x1B[36m"
#define TERM_YONR  "\x1b[94;41m"
#define TERM_BONW  "\x1B[30;47m"
#define TERM_RESET "\x1B[0m"

//useful macros for bit-ops
#define SET_BIT(num, bit)  (num) |= 1 << (bit)
#define CLEAR_BIT(num, bit)  (num) &= ~(1 << (bit))


#endif //SIMBLY_COMMON_H__
