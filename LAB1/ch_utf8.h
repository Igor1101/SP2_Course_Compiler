#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h>
#include "utf8.h"
typedef unsigned int ch_t;
ch_t ch_num(char* ch);
bool ch_is_same(ch_t ch1, ch_t ch2);
int ch_utf_strcmp(char* str1, char* str2);
int ch_utf_memcmp(char* str, char* key);
char* ch_utf_diff_alloc(char*str1, char*str2);


#endif/* MAIN_H */
