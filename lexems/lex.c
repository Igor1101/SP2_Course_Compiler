/*
 * lex.c
 *
 *  Created on: Sep 29, 2019
 *      Author: igor
 */
#include <defs.h>
#include <stdio.h>
#include <lexems/utf8.h>
#include <stdbool.h>
#include <string.h>
#include <lexems/MooreMachine.h>
#include <ctype.h>

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

bool is_keyword(char* str)
{
	for(int i=0; i< sizeof c_reservedwords / sizeof c_reservedwords[0]; i++) {
		if(!strcmp(str, c_reservedwords[i])) {
			return true;
		}
	}
	return false;
}

bool is_char_in(unsigned ch, const char*str)
{
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
	const char*acc_start =
			"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_";
	const char*acc =
			"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_1234567890";
	bool is_acc_char(char c) {
		return is_char_in(c, acc);
	}

	bool is_sacc_char(char c) {
		return is_char_in(c, acc_start);
	}
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

MOORE_SIGNAL ch_to_moore_signal(unsigned ch)
{
	if(is_char_in(ch, "1234567890")) {
		return SIG_DIGIT;
	}
	if(is_char_in(ch, "eE")) {
		return SIG_EXP;
	}
	if(is_char_in(ch, "1234567890abcdefABCDEF")) {
		return SIG_HEX;
	}
	if(is_char_in(ch, "\"")) {
		return SIG_STRBEG;
	}
	if(is_char_in(ch, ".")) {
		return SIG_DOT;
	}
	if(is_char_in(ch, "+-")) {
		return SIG_SIGN;
	}
	if(isspace(ch)) {
		return SIG_EMPTY;
	}
	if(is_char_in(ch, ";")) {
		return SIG_SEPOP;
	}
	if(is_char_in(ch, ":")) {
		return SIG_SEPGROUP;
	}
	if(is_char_in(ch, ",")) {
		return SIG_SEPLST;
	}
	if(is_char_in(ch, "<([{")) {
		return SIG_OPENBR;
	}
	if(is_char_in(ch, ">)]}")) {
		return SIG_CLOSEDBR;
	}
	return SIG_NC;
}

