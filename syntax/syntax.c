/*
 * analyzer.c
 *
 *  Created on: Oct 9, 2019
 *      Author: igor
 */
#include <lexems/lex.h>
#include <lexems/tables.h>
#include <defs.h>
#include "syntax.h"
#include <stdbool.h>
#include <string.h>

syn_state_t st;
int err_amount = 0;
void init_syn_analyzer(void)
{
	st.num = 0;
	st.arithmetic = false;
	st.assignment = false;
	st.curlybrace_nest = 0;
	st.nesting = 0;
	st.parensis_nest = 0;
	st.square_braces_nest = 0;
	st.symbols_after_del = 0;
	err_amount = 0;
}
int syn_analyze(void)
{
	init_syn_analyzer();
	for(st.num=0; st.num<str_array.amount; ) {
		if(str_get(st.num) == NULL) {
			pr_err("SYNTAX: NULL str_t detected");
			err_amount++;
			st.num++;
			continue;
		}
		if(str_get(st.num)->inst == NULL) {
			pr_err("SYNTAX: NULL string detected");
			err_amount++;
			st.num++;
			continue;
		}
		switch(str_get(st.num)->lext) {
		case L_IDENTIFIER:
			st.num = get_identifier_type(st.num, st.nesting, false);
			break;
		case L_BRACE_CLOSING:
			//close_brace(st.num);
		case L_BRACE_OPENING:
			//open_brace(st.num);
		default:
			str_get(st.num)->synt = S_NOTDEFINED;
			str_get(st.num)->level = st.nesting+1;
			st.num++;

		}
    }
	return err_amount;
}

int close_brace(int num)
{
	int ret;
	switch(*(str_get(num)->inst)) {
	case ')':
		ret = --st.parensis_nest;
		break;
	case '}':
		ret = --st.curlybrace_nest;
		break;
	case ']':
		ret = --st.parensis_nest;
		break;
	default:
		pr_warn("close_brace garbage detected");
	}
	return ret;
}

int open_brace(int num)
{
	int ret;
	switch(*(str_get(num)->inst)) {
	case '(':
		ret = ++st.parensis_nest;
		break;
	case '{':
		ret = ++st.curlybrace_nest;
		break;
	case '[':
		ret = ++st.parensis_nest;
		break;
	default:
		pr_warn("open_brace garbage detected %c", *(str_get(num)->inst));
	}
	return ret;
}

int push(syn_t op)
{
	if(st.__op_p >= MAX_DEPTH) {
		pr_err("op push overflow");
		return -1;
	}
	st.op[st.__op_p++] = op;
	return 0;
}
syn_t pop(void)
{
	if(st.__op_p == 0) {
		pr_err("op pop is underflow");
		return S_NOTDEFINED;
	}
	return st.op[st.__op_p--];
}

void reset(void)
{
	st.__op_p = 0;
}

int get_identifier_type(int num, int level, bool maybeparam)
{
	if(num < (str_array.amount-1)) {
		if(str_get(num+1)->lext == L_BRACE_OPENING &&
				!strcmp(str_get(num+1)->inst,"(")) {
			/* this is a function */
			pr_debug("function %s", str_get(num)->inst);
			int i = process_function(num, level);
			return i;
		}
		if(
				str_get(num+1)->lext == L_OPERAT_ARITHMETIC||
				str_get(num+1)->lext == L_OPERAT_ASSIGNMENT||
				str_get(num+1)->lext == L_OPERAT_BITWISE||
				str_get(num+1)->lext == L_OPERAT_LOGIC||
				str_get(num+1)->lext == L_OPERAT_RELATION) {
			str_get(num)->synt = S_ID_VARIABLE;
			str_get(num)->level = level+1;
			return ++num;
		}
		if(str_get(num+1)->lext == L_BRACE_CLOSING ||
				str_get(num+1)->lext == L_DELIMITER) {
			str_get(num)->synt = S_ID_PARAM;
			str_get(num)->level = level;
			return ++num;
		}
	}
	if(st.symbols_after_del > 0 && num > 0) {
		if(
				str_get(num-1)->lext == L_OPERAT_ARITHMETIC||
				str_get(num-1)->lext == L_OPERAT_ASSIGNMENT||
				str_get(num-1)->lext == L_OPERAT_BITWISE||
				str_get(num-1)->lext == L_OPERAT_LOGIC||
				str_get(num-1)->lext == L_OPERAT_RELATION) {
			str_get(num)->synt = S_ID_VARIABLE;
			str_get(num)->level = level;
			return ++num;
		}
	}
	err_amount++;
	str_get(num)->synt = S_ID_UNDEFINED;
	str_get(num)->level = level;
	return ++num;
}

char* syn_to_str(syn_t t)
{
	switch(t) {
	case S_NOTDEFINED:
		return "UNDEFINED";
	case S_ID_FUNCTION:
		return "ID_FUNCTION";
	case S_ID_UNDEFINED:
		return "ID_UNDEFINED";
	case S_ID_VARIABLE:
		return "ID_VARIABLE";
	case S_OPERAT_ARITHMETIC:
		return "OPERAT_ARITHMETIC";
	case S_OPERAT_ASSIGNMENT:
		return "OPERAT_ASSIGNMENT";
	case S_OPERAT_BITWISE:
		return "OPERAT_BITWISE";
	case S_OPERAT_IF:
		return "OPERAT_IF";
	case S_OPERAT_LOGIC:
		return "OPERAT_LOGIC";
	case S_OPERAT_RELATION:
		return "OPERAT_RELATION";
	case S_OPERAT_SWITCH_CASE:
		return "OPERAT_SWITCH_CASE";
	case S_OPERAT_UNARY:
		return "OPERAT_UNARY";
	case S_OPERAT_UNKNOWN:
		return "OPERAT_UNKNOWN";
	case S_DEL_PARAM:
		return "PARAM DELIMITER";
	case S_EXPECTED_FUNC_BRACE_CLOSE:
		return "ERR EXPECTED FUNC BRACE CLOSE";
	case S_EXPECTED_PARAM_DELIMITER:
		return "ERR EXPECTED PARAM DEL";
	case S_FUNC_BRACE_CLOSE:
		return "FUNC BRACE CLOSE";
	case S_FUNC_BRACE_OPEN:
		return "FUNC BRACE OPEN";
	case S_ID_PARAM:
		return "PARAM IDENT";
	case S_CONST_PARAM:
		return "CONST PARAM";
	default:
		return "URECOGNIZED";
	}
}

int process_function(int num, int level)
{
	bool has_del = false;
	int level_del = level+3;
	int level_param = level+3;
	if(str_get(num)->lext == L_IDENTIFIER) {
		str_get(num)->synt = S_ID_FUNCTION;
		str_get(num)->level = level+1;
	}
	if(str_get(num+1)->lext == L_BRACE_OPENING) {
		str_get(num+1)->synt = S_FUNC_BRACE_OPEN;
		str_get(num+1)->level = level+2;
		pr_debug("opening func brace");
	}
	for(int i=num+2; i<str_array.amount; ) {
		switch(str_get(i)->lext) {
		case L_BRACE_CLOSING:
			if(!strcmp(str_get(i)->inst, ")") ) {
				pr_debug("closing func brace");
				str_get(i)->synt = S_FUNC_BRACE_CLOSE;
				str_get(i)->level = level+2;
				return i+1;
			}
			/* unexpected brace */
			str_get(i)->synt = S_EXPECTED_FUNC_BRACE_CLOSE;
			err_amount++;
			return i+1;
		case L_IDENTIFIER:
		{
			int next = get_identifier_type(i, level_param, true);
			if(str_get(i)->synt == S_ID_PARAM) {
				pr_debug("it was param");
			}
			i = next;
			break;
		}
		case L_CHAR:
		case L_CONSTANT:
		case L_CONSTANT_BIN:
		case L_CONSTANT_FLOAT:
		case L_CONSTANT_HEX:
		case L_STRING:
			str_get(i)->synt = S_CONST_PARAM;
			str_get(i)->level = level_param;
			i++;
			break;
		case L_DELIMITER:
			if(!strcmp(str_get(i)->inst, ",")) {
				if(has_del == false) {
					/* modify settings */
					has_del = true;
					level_param = level+4;
					i = num+2;
					continue;
				}
				str_get(i)->synt = S_DEL_PARAM;
				str_get(i)->level = level_del;
				i++;
				break;
			}
			str_get(i)->synt = S_EXPECTED_PARAM_DELIMITER;
			i++;
			break;
		default:
			str_get(i)->synt = S_ID_UNDEFINED;
			str_get(i)->level = level_param;
			i++;
		}
	}
	pr_err("Expected ')' brace at end of input");
	err_amount++;
	return str_array.amount;
}
