/*
 * syn_utils.c
 *
 *  Created on: Oct 28, 2019
 *      Author: igor
 */
#include "syntax.h"
#include <lexems/lex.h>
#include <stdbool.h>
#include <string.h>
#include <lexems/tables.h>
#include <defs.h>

static str_t end_err = {
		.lex_err=false,
		.syn_err=false,
		.syn_inst_exp = ""
};

void set_synt(int num, syn_t s, int level)
{
	str_get(num)->synt = s;
	str_get(num)->syn_err = false;
	str_get(num)->synexp = s;
	str_get(num)->synunexp = S_NOTDEFINED;
	str_get(num)->level = level;
	str_get(num)->syn_inst_exp = "";
}
void set_synt_err_unexp(int num, syn_t expected, syn_t unexpected)
{
	str_get(num)->syn_err = true;
	str_get(num)->synexp = expected;
	str_get(num)->synunexp = unexpected;
	str_get(num)->syn_inst_exp = "";
}

void set_synt_err_unexp_inst(int num, syn_t expected, syn_t unexpected,
		char* inst)
{
	str_get(num)->syn_err = true;
	str_get(num)->synexp = expected;
	str_get(num)->synunexp = unexpected;
	str_get(num)->syn_inst_exp = inst;
}

void set_synt_err(int num, syn_t exp)
{
	set_synt_err_unexp(num, exp, lex_to_syn(str_get(num)->lext));
}

void set_synt_err_inst(int num, syn_t exp, char* inst)
{
	set_synt_err_unexp_inst(num, exp, lex_to_syn(str_get(num)->lext), inst);
}

void end_set_synt_err_inst(syn_t exp, char* inst)
{
	end_err.synexp = exp;
	end_err.syn_err = true;
	end_err.syn_inst_exp = inst;
}

str_t* end_get_synt_err(void)
{
	return &end_err;
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
	case S_KEYWORD:
		return "KEYWORD";
	case S_KEYWORD_TYPE:
		return "KEYWORD TYPE";
	case S_FUNC_BRACE_CLOSE:
		return "FUNC BRACE CLOSE";
	case S_CHAR:
		return "CHAR";
	case S_STRING:
		return "STRING";
	case S_POINTER:
		return "POINTER";
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
	}
	//return "ERROR UNRECOGNIZED SYMBOL";
}

void syn_printf(void)
{
	pr_info("syn output:");
	char *arr = malloc(1000);
	size_t *sz = malloc(str_array.amount*sizeof(size_t));
	size_t sz_cur = 0;

	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->syn_err) {
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
		if(str_get(i)->syn_err) {
			for(int j=0; j<sz[i]; j++)
				putchar(' ');
			pr_info("^\"%s\"<%s>, but expected \"%s\" <%s> ",
					str_get(i)->inst,
					syn_to_str(str_get(i)->synunexp),
					str_get(i)->syn_inst_exp,
					syn_to_str(str_get(i)->synexp));
		}
	}
	if(end_get_synt_err()->syn_err) {
		for(int j=0; j<sz[str_array.amount-1] + 1; j++)
			putchar(' ');
		pr_info("in the end expected \"%s\" <%s> ",
					end_get_synt_err()->syn_inst_exp,
					syn_to_str(end_get_synt_err()->synexp));
	}
	free(arr);
	free(sz);
}

int is_delimiter_next_expected(int num, int level, bool forcenext)
{
	if(num>=str_array.amount) {
		pr_err("DELIMITER EXPECTED AT THE END"
	"EOF FOUND");
	}
	else if(str_get(num)->lext == L_DELIMITER) {
		set_synt(num, S_DEL, level);
	} else {
		set_synt_err(num, S_DEL);
		if(!forcenext)
			return num;
	}
	return num+1;
}

void init_syn_analyzer(void)
{
	end_err.syn_err = false;
	end_err.lex_err = false;
	st.num = 0;
	st.arithmetic = false;
	st.assignment = false;
	st.curlybrace_nest = 0;
	st.nesting = 0;
	st.parensis_nest = 0;
	st.square_braces_nest = 0;
	st.symbols_after_del = 0;
}
