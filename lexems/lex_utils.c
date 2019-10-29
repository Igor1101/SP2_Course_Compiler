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
