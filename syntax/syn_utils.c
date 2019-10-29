/*
 * syn_utils.c
 *
 *  Created on: Oct 28, 2019
 *      Author: igor
 */
#include "syntax.h"
#include <lexems/lex.h>
#include <stdbool.h>
#include <lexems/tables.h>

void set_synt(int num, syn_t s)
{
	str_get(num)->synt = s;
	str_get(num)->syn_err = false;
	str_get(num)->synexp = s;
	str_get(num)->synunexp = S_NOTDEFINED;
}
void set_synt_err_unexp(int num, syn_t expected, syn_t unexpected)
{
	str_get(num)->syn_err = true;
	str_get(num)->synexp = expected;
	str_get(num)->synunexp = unexpected;
}

void set_synt_err(int num, syn_t exp)
{
	set_synt_err_unexp(num, exp, lex_to_syn(str_get(num)->lext));
}

syn_t lex_to_syn(lexem_t l)
{
	switch(l) {
	case L_BRACE_CLOSING:
		return S_BRACE_CLOSE;
	case L_BRACE_OPENING:
		return S_BRACE_OPEN;
	case L_CHAR:
		return S_CHAR;
	case L_CONSTANT:
	case L_CONSTANT_BIN:
	case L_CONSTANT_FLOAT:
	case L_CONSTANT_HEX:
		return S_CONST;
	case L_DELIMITER:
		return S_DEL;
	case L_IDENTIFIER:
		return S_ID_UNDEFINED;
	case L_KEYWORD:
		return S_KEYWORD;
	case L_NOTDEFINED:
		return S_NOTDEFINED;
	case L_OPERAT_ARITHMETIC:
		return S_OPERAT_ARITHMETIC;
	case L_OPERAT_ASSIGNMENT:
		return S_OPERAT_ASSIGNMENT;
	case L_OPERAT_BITWISE:
		return S_OPERAT_BITWISE;
	case L_OPERAT_LOGIC:
		return S_OPERAT_LOGIC;
	case L_OPERAT_RELATION:
		return S_OPERAT_RELATION;
	case L_STRING:
		return S_STRING;
	case L_POINTER:
	case L_STRUCT_DELIMITER:
	case L_STRUCT_POINTER:
		return S_POINTER;
	case L_UNACCEPTABLE_CHAR:
	case L_UNACCEPTABLE_WORD:
	case L_UNKNOWN_WORD:
		return S_ID_UNDEFINED;
	}
	return S_NOTDEFINED;
}
char* syn_to_str(syn_t t)
{
	switch(t) {
	case S_BRACE_CLOSE:
		return "BRACE CLOSE";
	case S_BRACE_OPEN:
		return "BRACE OPEN";
	case S_VARIABLE:
		return "VARIABLE";
	case S_CONST:
		return "CONSTANT";
	case S_DEL:
		return "DELIMITER";
	case S_NOTDEFINED:
		return "NOT PROCESSED";
	case S_ID_FUNCTION:
		return "ID FUNCTION";
	case S_ID_UNDEFINED:
		return "ERR ID UNDEFINED";
	case S_ID_VARIABLE:
		return "ID VARIABLE";
	case S_OPERAT_ARITHMETIC:
		return "OPERAT ARITHMETIC";
	case S_OPERAT_ASSIGNMENT:
		return "OPERAT ASSIGNMENT";
	case S_OPERAT_BITWISE:
		return "OPERAT BITWISE";
	case S_OPERAT_IF:
		return "OPERAT IF";
	case S_OPERAT_BINARY:
		return "OPERAT BINARY";
	case S_OPERAT_LOGIC:
		return "OPERAT_LOGIC";
	case S_OPERAT_RELATION:
		return "OPERAT_RELATION";
	case S_OPERAT_SWITCH_CASE:
		return "OPERAT_SWITCH_CASE";
	case S_OPERAT_UNARY:
		return "OPERAT_UNARY";
	case S_DEL_PARAM:
		return "PARAM DELIMITER";
	case S_FUNC_BRACE_OPEN:
		return "FUNC BRACE OPEN";
	case S_ID_PARAM:
		return "PARAM IDENT";
	case S_CONST_PARAM:
		return "CONST PARAM";
	//default:
	//	return "UNRECOGNIZED";
	}
	return "ERROR UNRECOGNIZED SYMBOL";
}
