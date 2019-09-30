/*
 * lex.c
 *
 *  Created on: Sep 29, 2019
 *      Author: igor
 */
#include <defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <lexems/utf8.h>
#include <lexems/tables.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

static const char * c_reservedwords[] = {
		"auto", "signed", "const", "extern", "register", "unsigned",
		"int", "float", "char", "double", "long",
		"void",
		"if", "else", "switch", "case", "default",
		"break", "continue",
		"for", "while", "do",
		"goto",
		"return",
		"enum",
		"sizeof",
		"struct", "typedef", "union",
		"volatile"
};

static const char* c_operators[] = {
		"+", "-", "*", "/", "%",
		"++", "--",
		"=", "+=", "-=", "*=", "/=", "%=",
		"==", ">","<", "!=", ">=", "<=",
		"&&", "||", "!",
		"&", "|", "^", "~", "<<", ">>"
};

bool is_keyword(char* str)
{
	if(str == NULL) {
		return NULL;
	}
	for(int i=0; i< sizeof c_reservedwords / sizeof c_reservedwords[0]; i++) {
		if(!strcmp(str, c_reservedwords[i])) {
			return true;
		}
	}
	return false;
}

bool is_char_in(unsigned ch, const char*str)
{
	if(str == NULL) {
		return false;
	}
	if(ch > 255) {
		return false;/* dont support utf-8 */
	}
	for(int i=0; i<strlen(str); i++) {
		if(ch == str[i]) {
			return true;
		}
	}
	return false;
}

bool is_name(char* str)
{

	if(str == NULL)
		return false;
	if(!is_sacc_char(str[0]))
		return false;
	for(int i=0; i<strlen(str); i++) {
		if(!is_acc_char(str[i])) {
			return false;
		}
	}
	return true;
}

char* get_next_lexem_alloc(char*str, int* i)
{
	char*lex;
	const char* op_ch = "?!~|%^&*-+=/<>";
	if(str == NULL)
		return NULL;
	unsigned ch = u8_nextchar(str, i);
	pr_debug("char %c indentifying", ch);
	if(is_char_in(ch, ";,. ")) {
		pr_debug("lex is delimiter");
		lex = calloc(1, 1);
		lex[0] = (char)ch;
		return lex;
	}
	if(is_char_in(ch, op_ch)) {
		pr_debug("lexem may be C operator");
		lex = calloc(1, 1);
		int index = 0;
		do {
			lex = reallocarray(lex, index+1, 1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
		} while(is_char_in(ch, op_ch));
		pr_debug("alloc %s", lex);
		/* set null char */
		lex = reallocarray(lex, index+1, 1);
		lex[index] = '\0';
		return lex;
	}
	if(is_sacc_char(ch)) {
		/* this may be identifier */
		lex = calloc(1, 1);
		int index = 0;
		do {
			lex = reallocarray(lex, index+1, 1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
			pr_debug("char is %c", ch);
		} while(is_acc_char(ch));
		/* set null char */
		lex = reallocarray(lex, index+1, 1);
		lex[index] = '\0';
		pr_debug("alloc %s", lex);
		return lex;
	}
}


int lex_parse(char*str)
{
	str_array_remove();
	bool inside_quat = false;
	bool inside_lexem = false;
	int ret_status;
	int i = 0;
	while(1) {
		unsigned ch = u8_nextchar(str, &i);
		if(ch == 0) {
			break;
		}
		if(ch > 255 && (inside_quat == false)) {
			pr_err("non ascii character detected outside quatation");
		}
		if(inside_lexem == false) {
			/* may be we are just at the beginning */
			u8_dec(str, &i);
			char* lex = get_next_lexem_alloc(str, &i);
			if(lex == NULL) {
				return ret_status;
			}
			/* parse lexem to find out what is it */
			if(is_keyword(lex)) {
				pr_debug("found keyword");
				str_add(lex, L_KEYWORD);
			} else if(is_name(lex)) {
				pr_debug("found identifier");
				str_add(lex, L_IDENTIFIER);
			}
			free(lex);
		}
	}
	return ret_status;
}

const char*acc_start =
			"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_";
const char*acc =
			"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_1234567890";
bool is_acc_char(char c)
{
	return is_char_in(c, acc);
}

bool is_sacc_char(char c)
{
	return is_char_in(c, acc_start);
}

char* lex_to_str(lexem_t lt)
{
	switch(lt) {
	case  L_CONSTANT:
		return "constant";
	case L_CONSTANT_BIN:
		return "constant binary";
	case L_CONSTANT_HEX:
		return "constant hexademical";
	case L_IDENTIFIER:
		return "identifier";
	case L_KEYWORD:
		return "ckeyword";
	case L_OPERAT_ARITHMETIC:
		return "arithmetic op";
	case L_OPERAT_ASSIGNMENT:
		return "assignment op";
	case L_OPERAT_BITWISE:
		return "bitwise op";
	case L_OPERAT_LOGIC:
		return "logic op";
	case L_OPERAT_RELATION:
		return "relation op";
	case L_UNACCEPTABLE_CHAR:
		return "ERR UNACCEPTABLE CHAR";
	case L_UNACCEPTABLE_WORD:
		return "ERR UNACCEPTABLE WORD";
	case L_UNKNOWN_WORD:
		return "ERR UNKNOWN WORD";
	default:
		return "UNKNOWN LEXEM";
	}
}
