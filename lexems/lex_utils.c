/*
 * lex_utils.c
 *
 *  Created on: Oct 29, 2019
 *      Author: igor
 */
#include "lex.h"
#include <lexems/tables.h>
#include <defs.h>
#include <stdlib.h>
#include <string.h>

char* lex_to_str(lexem_t lt)
{
	switch(lt) {
	case L_STRING:
		return "STRING";
	case L_CHAR:
		return "CHAR";
	case L_STRUCT_DELIMITER:
		return "STRUCT DELIMITER";
	case L_STRUCT_POINTER:
		return "STRUCT POINTER";
	case L_POINTER:
		return "POINTER";
	case L_CONSTANT_FLOAT:
		return "CONSTANT FLOAT";
	case L_DELIMITER:
		return "DELIMITER";
	case L_BRACE_CLOSING:
		return "CLOSING BRACE";
	case L_BRACE_OPENING:
		return "OPENING BRACE";
	case  L_CONSTANT:
		return "CONSTANT";
	case L_CONSTANT_BIN:
		return "CONSTANT BINARY";
	case L_CONSTANT_HEX:
		return "CONSTANT HEXADEMICAL";
	case L_IDENTIFIER:
		return "IDENTIFIER";
	case L_KEYWORD:
		return "KEYWORD";
	case L_OPERAT_ARITHMETIC:
		return "ARITHMETIC OP";
	case L_OPERAT_ASSIGNMENT:
		return "ASSIGNMENT OP";
	case L_OPERAT_BITWISE:
		return "BITWISE OP";
	case L_OPERAT_LOGIC:
		return "LOGIC OP";
	case L_OPERAT_RELATION:
		return "RELATION OP";
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

void lex_add(char* str, lexem_t l)
{
	int ind = str_add(str, l);
	switch (l) {
		case L_UNACCEPTABLE_CHAR:
		case L_UNACCEPTABLE_WORD:
		case L_UNKNOWN_WORD:
			str_get(ind)->lex_err=true;
			break;
		default:
			str_get(ind)->lex_err=false;
			break;
	}
}

int lex_err_amount(void)
{
	int err=0;
	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->lex_err)
			err++;
	}
	return err;
}

void lex_printf(void)
{
	pr_info("lex output:");
	char *arr = malloc(1000);
	size_t *sz = malloc(str_array.amount*sizeof(size_t));
	size_t sz_cur = 0;

	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->lex_err) {
			printf(COLOR_RED " %s" COLOR_DEF, str_get_inst(i));
		} else {
			printf(COLOR_GREEN " %s" COLOR_DEF, str_get_inst(i));
		}
		sprintf(arr, " %s" , str_get_inst(i));
		sz[i] = strlen(arr) + sz_cur;
		sz_cur = sz[i];
	}
	puts("");
	for(int i=0; i<str_array.amount;i++) {
		if(str_get(i)->lex_err) {
			for(int j=0; j<sz[i]; j++)
				putchar(' ');
			pr_info("^ lexical error <%s> ", lex_to_str(str_get(i)->lext));
		}
	}
	free(arr);
	free(sz);
}
