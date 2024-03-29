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
		"auto", "signed", "const", "extern", "register", "unsigned", "static",
		"int", "float", "char", "double", "long", "short",
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
static const char* c_op_arithmetic[] = {
		"+", "-", "*", "/", "%", "++", "--"
};
static const char* c_op_assign[] = {
		"=", "+=", "-=", "*=", "/=", "%="
};
static const char* c_op_bitwise[] = {
		"&", "|", "^", "~", "<<", ">>"
};
static const char* c_op_logic[] = {
		"&&", "||", "!",
};
static const char* c_op_relation[] = {
		"==", ">","<", "!=", ">=", "<=",
};

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
			if(ch > 255 && ch != EOF) {
				ch = '?';
				*lexerror = L_UNACCEPTABLE_CHAR;
			}
			lex = str_realloc(lex, index+1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
		} while((ch != 0 ) &&
				(!isspace(ch)) &&
				(!is_char_in(ch, ";,:")) &&
				(!is_char_in(ch, op_ch)) &&
				(!is_char_in(ch, "()[]{}<>")));
		/* set null char */
		lex = str_realloc(lex, index+1);
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
			lex = str_realloc(lex, index+1);
			lex[index++] = ch;
			ch = u8_nextchar(str, i);
		} while(is_char_in(ch, op_ch));
		pr_debug("alloc %s", lex);
		/* set null char */
		lex = str_realloc(lex, index+1);
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
	lexem_t lastlexem = L_NOTDEFINED;
	int ret_status = 0;
	int i = 0;
	while(1) {
		unsigned ch = u8_nextchar(str, &i);
		if(ch == 0) {
			break;
		}
		if(ch > 255 && ch != EOF) {
			pr_err("non ascii character detected outside quatation =%u", ch);
			break;
		}
		if(is_char_in(ch, ";,:")) {
			pr_debug("found delimiter");
			char _del[2];
			_del[0] = ch;
			_del[1] = 0;
			str_add(_del, L_DELIMITER);
			lastlexem = L_DELIMITER;
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
			lastlexem = L_BRACE_OPENING;
			continue;
		}
		if(is_char_in(ch, ")]}")) {
			pr_debug("found closing brace");
			char _br[2];
			_br[0] = ch;
			_br[1] = 0;
			str_add(_br, L_BRACE_CLOSING);
			lastlexem = L_BRACE_CLOSING;
			continue;
		}
		if(is_char_in(ch, "'")) {
			/* looks like char symbol */
			pr_debug("looks like char");
			unsigned ch1 = u8_nextchar(str, &i);
			char ch2 = u8_nextchar(str, &i);
			if(is_char_in(ch2, "'") && ch1 < 255) {
				/* it is char symbol */
				char s[4] = { ch, (char)ch1, ch2, 0};
				str_add(s, L_CHAR);
				lastlexem = L_CHAR;
				continue;
			} else {
				u8_dec(str, &i);
				u8_dec(str, &i);
			}
		}
		if(is_char_in(ch, "\"")) {
			/* looks like string */
			pr_debug("string process");
			/* save str pointer */
			char* strend = strchr(&str[i+1], '"');
			if(strend == NULL) {
				pr_err("string without end \" ");
			} else {
				char*newstr = str_alloc(&str[i - 1]);
				char*newstrend = strchr(newstr+1, '"');
				if(newstrend == NULL) {
					free(newstr);
				} else {
					*(newstrend+1) = '\0';
					str_add(newstr, L_STRING);
					lastlexem = L_STRING;
					for(int j=0; j<u8_strlen(newstr)-1;j++) {
						u8_inc(str, &i);
					}
					continue;
				}
			}
		}
		/* parse lexem to find out what is it */
		lexem_t lerror ;
		/* may be we are just at the beginning
		 * of lexem */
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
		if(is_str_in(lex, c_reservedwords, sizeof c_reservedwords)) {
			pr_debug("found keyword");
			str_add(lex, L_KEYWORD);
			lastlexem = L_KEYWORD;
		} else if(is_name(lex)) {
			pr_debug("found identifier");
			str_add(lex, L_IDENTIFIER);
			lastlexem = L_IDENTIFIER;
		} else if(is_hex(lex)) {
			pr_debug("found hex number");
			str_add(lex, L_CONSTANT_HEX);
			lastlexem = L_CONSTANT_HEX;
		} else if(is_dec(lex)) {
			pr_debug("found decimal number");
			str_add(lex, L_CONSTANT);
			lastlexem = L_CONSTANT;
		} else if(is_op_chars(lex)) {
			pr_debug("may be operator");
			while(strlen(lex) > 0) {
				pr_debug("lex=%s", lex);
				if(is_str_in(lex, c_operators, sizeof c_operators)) {
					if(is_str_in(lex, c_op_arithmetic, sizeof c_op_arithmetic)) {
						/* verify if it is arithmetic or
						 * pointer
						 */
						if(!strcmp(lex, "*")) {
							if(lastlexem == L_BRACE_OPENING ||
									lastlexem == L_DELIMITER ||
									lastlexem == L_NOTDEFINED ||
									lastlexem == L_KEYWORD ||
									lastlexem == L_OPERAT_ARITHMETIC||
									lastlexem == L_OPERAT_ASSIGNMENT||
									lastlexem == L_OPERAT_BITWISE||
									lastlexem == L_OPERAT_LOGIC||
									lastlexem == L_OPERAT_RELATION||
									lastlexem == L_POINTER) {
								/* this looks like pointer! */
								pr_debug("pointer found!");
								str_add(lex, L_POINTER);
								lastlexem = L_POINTER;
								break;
							}
						}
						str_add(lex, L_OPERAT_ARITHMETIC);
						lastlexem = L_OPERAT_ARITHMETIC;
						break;
					}
					if(is_str_in(lex, c_op_assign, sizeof c_op_assign)) {
						str_add(lex, L_OPERAT_ASSIGNMENT);
						lastlexem = L_OPERAT_ASSIGNMENT;
						break;
					}
					if(is_str_in(lex, c_op_bitwise, sizeof c_op_bitwise)) {
					/* verify if it is arithmetic or
					* pointer
					*/
						if(!strcmp(lex, "&")) {
							if(lastlexem == L_BRACE_OPENING ||
									lastlexem == L_DELIMITER ||
									lastlexem == L_NOTDEFINED ||
									lastlexem == L_KEYWORD ||
									lastlexem == L_OPERAT_ARITHMETIC||
									lastlexem == L_OPERAT_ASSIGNMENT||
									lastlexem == L_OPERAT_BITWISE||
									lastlexem == L_OPERAT_LOGIC||
									lastlexem == L_OPERAT_RELATION) {
								/* this looks like pointer! */
								pr_debug("pointer found!");
								str_add(lex, L_POINTER);
								lastlexem = L_POINTER;
								break;
							}
						}
						str_add(lex, L_OPERAT_BITWISE);
						lastlexem = L_OPERAT_BITWISE;
						break;
					}
					if(is_str_in(lex, c_op_logic, sizeof c_op_logic)) {
						str_add(lex, L_OPERAT_LOGIC);
						lastlexem = L_OPERAT_LOGIC;
						break;
					}
					if(is_str_in(lex, c_op_relation, sizeof c_op_relation)) {
						str_add(lex, L_OPERAT_RELATION);
						lastlexem = L_OPERAT_RELATION;
						break;
					}
					const char*struct_p[] = {"->" };
					if(is_str_in(lex, struct_p, sizeof struct_p)){
						/* struct pointer  */
						str_add(lex, L_STRUCT_POINTER);
						lastlexem = L_STRUCT_POINTER;
						break;
					}
					const char*struct_elem[] = {"." };
					if(is_str_in(lex, struct_elem, sizeof struct_elem)){
						/* if point is standalone
						 * */
						str_add(lex, L_UNKNOWN_WORD);
						lastlexem = L_UNKNOWN_WORD;
						ret_status++;
						break;
					}
				}
				if(strlen(lex) == 1) {
					str_add(lex, L_UNKNOWN_WORD);
					lastlexem = L_UNKNOWN_WORD;
					ret_status++;
					break;
				}
				lex[strlen(lex)-1] = '\0';
				i--;
			}
		} else if(is_float(lex)){
			pr_debug("found floating number");
			str_add(lex, L_CONSTANT_FLOAT);
			lastlexem = L_CONSTANT_FLOAT;
		} else if(is_structident(lex)) {
			pr_debug("found struct element");
			/* separate parts of struct element */
			char*next_word = lex;
			char*point = ".";
			char*next_point = strchr(next_word, '.');
			while(next_point != NULL) {
				*next_point = '\0';
				str_add(next_word, L_IDENTIFIER);
				str_add(point, L_STRUCT_DELIMITER);
				next_word = next_point+1;
				next_point = strchr(next_word, '.');
			}
			str_add(next_word, L_IDENTIFIER);
		}
		else {
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
bool is_float(char*lex)
{
	bool point_once = false;
	bool exp_once = false;
	if(lex == NULL)
		return false;
	for(int i=0; i<strlen(lex); i++) {
		if(!isdigit(lex[i])) {
			if(lex[i] == '.' && i<(strlen(lex) - 1)) {
				if(point_once)
					return false;
				else {
					point_once = true;
				}
			} else if((lex[i] == 'e' || lex[i] == 'E') && i<strlen(lex) - 1){
				if(exp_once)
					return false;
				else {
					exp_once = true;
				}
			} else if((lex[i] == '-' || lex[i] == '+') && i<strlen(lex) - 1 &&
					i > 1) {
				if(lex[i-1] == 'E' || lex[i-1] == 'e') {
					continue;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
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

bool is_str_in(char*str, const char*arr[], size_t arr_size)
{
	if(str == NULL) {
		return false;
	}
	for(int i=0; i<arr_size / sizeof arr[0]; i++) {
		if(!strcmp(str, arr[i]))
			return true;
	}
	return false;
}

bool is_structident(char*str)
{
	bool lastp_near = false;
	bool is_point(char ch)
	{
		if(ch=='.') {
			if(lastp_near) {
				return false;
			} else {
				lastp_near = true;
				return true;
			}
		} else {
			lastp_near = false;
		}
		return false;
	}
	if(!is_sacc_char(str[0])) {
		return false;
	}
	if((str[strlen(str)-1] == '.')) {
		return false;
	}
	for(int i=1; i<strlen(str); i++) {
		if((!is_acc_char(str[i])) && (!is_point(str[i])))
			return false;
		is_point(str[i]);
	}
	return true;
}
