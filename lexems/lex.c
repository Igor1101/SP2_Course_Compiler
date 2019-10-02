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
		"auto", "signed", "const", "extern", "register", "unsigned", "static"
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
		"&", "|", "^", "~", "<<", ">>", "->", "."
};

bool is_keyword(char* str)
{
	if(str == NULL) {
		return NULL;
	}
	for(int i=0; i < sizeof c_reservedwords / sizeof c_reservedwords[0]; i++) {
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

char* get_next_lexem_alloc(char*str, int* i, lexem_t* lexerror)
{
	char*lex;
	const char* op_ch = "?!~|%^&*-+=/<>";
	if(str == NULL)
		return NULL;
	unsigned ch = u8_nextchar(str, i);
	pr_debug("char %c indentifying", ch);
	if(is_char_in(ch, ";,:. ")) {
		pr_debug("lex is delimiter");
		lex = calloc(1, 2);
		lex[0] = (char)ch;
		lex[1] = 0;
		return lex;
	}
	if(is_char_in(ch, "()[]{}")) {
		pr_debug("lex is brace");
		lex = calloc(1, 2);
		lex[0] = (char)ch;
		lex[1] = 0;
		return lex;
	}
	char* get_full_lexem(unsigned ch)
	{
		char*lex = calloc(1, 1);
		int index = 0;
		do {
			if(ch > 255) {
				ch = '?';
				*lexerror = L_UNACCEPTABLE_CHAR;
			}
			lex = reallocarray(lex, index+1, 1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
		} while((ch != 0 ) &&
				(!isspace(ch)) &&
				(!is_char_in(ch, ";,:")) &&
				(!is_char_in(ch, op_ch)) &&
				(!is_char_in(ch, "()[]{}<>")));
		/* set null char */
		lex = reallocarray(lex, index+1, 1);
		lex[index] = '\0';
		/* move to previous char */
		u8_dec(str, i);
		pr_debug("alloc %s", lex);
		return lex;
	}
	if(is_char_in(ch, op_ch)) {
		pr_debug("lexem may be C operator");
		lex = calloc(1, 1);
		int index = 0;
		do {
			if(ch > 255) {
				ch = '?';
				*lexerror = L_UNACCEPTABLE_CHAR;
			}
			lex = reallocarray(lex, index+1, 1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
		} while(is_char_in(ch, op_ch));
		pr_debug("alloc %s", lex);
		/* set null char */
		lex = reallocarray(lex, index+1, 1);
		lex[index] = '\0';
		/* move to previous char */
		u8_dec(str, i);
		return lex;
	}
	if(is_sacc_char(ch)) {
		/* this may be identifier */
		return get_full_lexem(ch);
	}
	if(is_char_in(ch, "0123456789") ) {
		/* does look like dec binary or hex number */
		return get_full_lexem(ch);
	}
	/* something is unknown */
	lex = calloc(1,2);
	lex[0] = ch;
	lex[1] = 0;
	return lex;
}


int lex_parse(char*str)
{
	str_array_remove();
	int ret_status = 0;
	int i = 0;
	while(1) {
		unsigned ch = u8_nextchar(str, &i);
		if(ch == 0) {
			break;
		}
		if(ch > 255 ) {
			pr_err("non ascii character detected outside quatation");
			ret_status++;
			continue;
		}
		if(is_char_in(ch, ";,:")) {
			pr_debug("found delimiter");
			char _del[2];
			_del[0] = ch;
			_del[1] = 0;
			str_add(_del, L_DELIMITER);
			continue;
		}
		if(isspace(ch)) {
			pr_debug("ignoring space");
			continue;
		}
		if(is_char_in(ch, "([{")) {
			pr_debug("found opening brace");
			char _br[2];
			_br[0] = ch;
			_br[1] = 0;
			str_add(_br, L_BRACE_OPENING);
			continue;
		}
		if(is_char_in(ch, ")]}")) {
			pr_debug("found closing brace");
			char _br[2];
			_br[0] = ch;
			_br[1] = 0;
			str_add(_br, L_BRACE_CLOSING);
			continue;
		}
		/* parse lexem to find out what is it */
		lexem_t lerror ;
		/* may be we are just at the beginning */
		u8_dec(str, &i);
		lerror = L_NOTDEFINED;
		char* lex = get_next_lexem_alloc(str, &i, &lerror);
		if(lex == NULL) {
			return ret_status;
		}
		if(lerror == L_UNACCEPTABLE_CHAR || lerror == L_UNACCEPTABLE_WORD) {
			str_add(lex, lerror);
			ret_status++;
			continue;
		}
		if(is_keyword(lex)) {
			pr_debug("found keyword");
			str_add(lex, L_KEYWORD_);
		} else if(is_name(lex)) {
			pr_debug("found identifier");
			str_add(lex, L_IDENTIFIER);
		} else if(is_hex(lex)) {
			pr_debug("found hex number");
			str_add(lex, L_CONSTANT_HEX);
		} else if(is_dec(lex)) {
			pr_debug("found decimal number");
			str_add(lex, L_CONSTANT);
		} else if(is_op_chars(lex)) {
			pr_debug("may be operator");
			while(strlen(lex) > 0) {
				pr_debug("lex=%s", lex);
				if(is_operator(lex)) {
					str_add(lex, L_OPERAT_ARITHMETIC);
					break;
				}
				lex[strlen(lex)-1] = '\0';
				i--;
			}
		} else {
			pr_warn("unknown word detected");
			str_add(lex, L_UNKNOWN_WORD);
			ret_status++;
		}
		free(lex);
	}
	return ret_status;
}

bool is_hex(char* lex)
{
	if(lex == NULL)
		return false;
	if(strlen(lex)<3)
		return false;
	if(lex[0] != '0' || (lex[1] != 'x' && lex[1] != 'X'))
		return false;
	for(int i=2; i<strlen(lex); i++) {
		if(!is_char_in(lex[i], "1234567890abcdefABCDEF")) {
			return false;
		}
	}
	return true;
}

bool is_dec(char*lex)
{
	if(lex == NULL)
		return false;
	for(int i=0; i<strlen(lex); i++) {
		if(!isdigit(lex[i]))
			return false;
	}
	return true;
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

bool is_op_chars(char*str)
{
	for(int i=0; i<strlen(str); i++) {
		if(!is_char_in(str[i], ".~|&!-+/<>=^%*")) {
			return false;
		}
	}
	return true;
}
bool is_operator(char*str)
{
	for(int i=0; i<sizeof c_operators / sizeof c_operators[0]; i++) {
		if(!strcmp(str, c_operators[i]))
			return true;
	}
	return false;
}

char* lex_to_str(lexem_t lt)
{
	switch(lt) {
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
	case L_KEYWORD_:
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
