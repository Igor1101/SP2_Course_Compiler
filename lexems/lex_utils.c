/*
 * lex_utils.c
 *
 *  Created on: Oct 29, 2019
 *      Author: igor
 */
#include "lex.h"

char* lex_to_str(lexem_t lt)
{
	switch(lt) {
	case L_STRING:
		return "string";
	case L_CHAR:
		return "char";
	case L_STRUCT_DELIMITER:
		return "struct delimiter";
	case L_STRUCT_POINTER:
		return "struct pointer";
	case L_POINTER:
		return "pointer";
	case L_CONSTANT_FLOAT:
		return "constant float";
	case L_DELIMITER:
		return "delimiter";
	case L_BRACE_CLOSING:
		return "closing brace";
	case L_BRACE_OPENING:
		return "opening brace";
	case  L_CONSTANT:
		return "constant";
	case L_CONSTANT_BIN:
		return "constant binary";
	case L_CONSTANT_HEX:
		return "constant hexademical";
	case L_IDENTIFIER:
		return "identifier";
	case L_KEYWORD:
		return "PASkeyword";
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
