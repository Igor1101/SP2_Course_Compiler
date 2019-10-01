/*
 * lex.h
 *
 *  Created on: Sep 29, 2019
 *      Author: igor
 */

#ifndef LEXEMS_LEX_H_
#define LEXEMS_LEX_H_

#include <stdbool.h>

typedef enum {
	L_NOTDEFINED,
	L_BRACE_OPENING,
	L_BRACE_CLOSING,
	L_DELIMITER,
	L_KEYWORD_,
	L_IDENTIFIER,
	L_CONSTANT,
	L_CONSTANT_HEX,
	L_CONSTANT_BIN,
	L_OPERAT_RELATION,
	L_OPERAT_ARITHMETIC,
	L_OPERAT_ASSIGNMENT,
	L_OPERAT_LOGIC,
	L_OPERAT_BITWISE,
	L_UNKNOWN_WORD,
	L_UNACCEPTABLE_CHAR,
	L_UNACCEPTABLE_WORD
}lexem_t;

bool is_keyword(char* str);
bool is_name(char* str);
bool is_char_in(unsigned ch, const char*str);
bool is_acc_char(char c);
bool is_sacc_char(char c);
int lex_parse(char*str);
char* get_next_lexem_alloc(char*str, int* i, lexem_t* lexerror);
char* lex_to_str(lexem_t lt);
bool is_hex(char* lex);
bool is_dec(char*lex);




#endif /* LEXEMS_LEX_H_ */
