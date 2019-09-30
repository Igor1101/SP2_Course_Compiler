/*
 * lex.h
 *
 *  Created on: Sep 29, 2019
 *      Author: igor
 */

#ifndef LEXEMS_LEX_H_
#define LEXEMS_LEX_H_

#include <stdbool.h>

bool is_keyword(char* str);
bool is_name(char* str);
bool is_char_in(unsigned ch, const char*str);
bool is_acc_char(char c);
bool is_sacc_char(char c);
int lex_parse(char*str);
char* get_next_lexem_alloc(char*str, int* i);



#endif /* LEXEMS_LEX_H_ */
