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
#include <stdlib.h>
#include <stddef.h>

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
	process_main(0, 0, false);
	return err_amount;

}

int process_main(int num, int level, bool inside_block)
{
	for(; num<str_array.amount; ) {
		if(str_get(num) == NULL) {
			pr_err("SYNTAX: NULL str_t detected");
			err_amount++;
			num++;
			continue;
		}
		if(str_get(num)->inst == NULL) {
			pr_err("SYNTAX: NULL string detected");
			err_amount++;
			num++;
			continue;
		}
		int prev = num;
		switch(str_get(num)->lext) {
		case L_KEYWORD:
			if(!strcasecmp(str_get_inst(num), "for")) {
				num = process_for_loop(num, level+1);
				break;
			}
			if(!strcasecmp(str_get_inst(num), "begin")) {
				num = process_block(num, level+1);
				break;
			}
			if(!strcasecmp(str_get_inst(num), "end") && inside_block) {
				set_synt(num, S_KEYWORD, level-1);
				return num+1;
			}
			break;
		case L_IDENTIFIER:
			num = process_ident(num, level+1, false, false);
			break;
		case L_OPERAT_ARITHMETIC:
			num = process_expression(num, level+1, false, false);
			break;
		case L_BRACE_CLOSING:
			//close_brace(num);
		case L_BRACE_OPENING:
			//open_brace(num);
		case L_DELIMITER:
			num = is_delimiter_next_expected(num, level, true);
			break;
		default:
			pr_err("UNEXPECTED SYMBOL");

			set_synt_err_unexp(num, S_NOTDEFINED, lex_to_syn(str_get(num)->lext));
			err_amount++;
			num++;
		}
		if(num >= str_array.amount) {
			if(str_array.amount>0 && (str_get(str_array.amount - 1)->lext != L_DELIMITER)) {
				set_synt_err(str_array.amount - 1, S_DEL);
				err_amount++;
			}
			return err_amount;
		}
		int next = num;
		num = is_delimiter_next_expected(num, level, next == prev);
    }
	if(str_array.amount>0 && (str_get(str_array.amount - 1)->lext != L_DELIMITER)) {
		if(!str_get(str_array.amount - 1)->syn_err) {
			set_synt_err(str_array.amount - 1, S_DEL);
			err_amount++;
		}
	}
	return num;
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

int process_ident(int num, int level, bool maybeparam, bool inside_expr)
{
	if(num < (str_array.amount-1)) {

		if(str_get(num)->lext == L_IDENTIFIER &&
			!strcmp(str_get(num + 1)->inst, "[")) {
			int i = process_array(num, level);
			return i;
		}
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
				str_get(num+1)->lext == L_OPERAT_RELATION||
				str_get(num-1)->lext == L_OPERAT_ARITHMETIC||
				str_get(num+1)->lext == L_OPERAT_ASSIGNMENT||
				str_get(num-1)->lext == L_OPERAT_BITWISE||
				str_get(num-1)->lext == L_OPERAT_LOGIC||
				str_get(num-1)->lext == L_OPERAT_RELATION||
				str_get(num+1)->lext == L_OPERAT_ASSIGNMENT
				) {
			if(inside_expr) {
				set_synt(num, S_ID_VARIABLE, level);
				return ++num;
			} else {
				return process_expression(num, level, maybeparam, false);
			}
		}
		if(str_get(num+1)->lext == L_KEYWORD) {
			set_synt(num, S_ID_VARIABLE, level);
			return ++num;
		}
		if(maybeparam && (str_get(num+1)->lext == L_BRACE_CLOSING ||
				str_get(num+1)->lext == L_DELIMITER)) {
			set_synt(num, S_ID_VARIABLE, level);
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
			set_synt(num, S_ID_VARIABLE, level);
			return ++num;
		}
	}
	err_amount++;
	set_synt_err_unexp(num, S_ID_VARIABLE, S_ID_UNDEFINED);
	return ++num;
}


int process_function(int num, int level)
{
	bool has_del = false;
	int level_del = level+2;
	int level_param = level+3;
	if(str_get(num)->lext == L_IDENTIFIER) {
		set_synt(num, S_ID_FUNCTION, level);
	}
	if(str_get(num+1)->lext == L_BRACE_OPENING) {
		set_synt(num+1, S_FUNC_BRACE_OPEN, level+1);
		pr_debug("opening func brace");
	}
	int next_del = next_delimiter(num, level, false);
	if(next_del<0) {
		pr_warn("could not find next delimiter");
		next_del = str_array.amount;
	}
	for(int i=num+2; i<next_del; ) {
		switch(str_get(i)->lext) {
		case L_BRACE_CLOSING:
			if(!strcmp(str_get(i)->inst, ")") ) {
				pr_debug("closing func brace");
				set_synt(i, S_FUNC_BRACE_CLOSE, level + 1);
				return i+1;
			}
			/* unexpected brace */
			set_synt_err(num, S_FUNC_BRACE_CLOSE);

			err_amount++;
			return i+1;
		case L_IDENTIFIER:
		{
			int next = process_ident(i, level_param, true, false);
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
			set_synt(i, S_CONST_PARAM, level_param);
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
				set_synt(i, S_DEL_PARAM, level_del);
				i++;
				break;
			}
			set_synt_err_unexp(i, S_FUNC_BRACE_CLOSE, S_DEL_PARAM);
			i++;
			break;
		default:
			set_synt_err_unexp(i, S_ID_UNDEFINED, lex_to_syn(str_get(i)->lext));
			i++;
		}
	}
	set_synt_err(next_del, S_BRACE_CLOSE);
	pr_err("Expected ')' brace at end of input");
	err_amount++;
	return str_array.amount;
}

int process_expression(int num, int level, bool inside_expr, bool inside_array)
{
	syn_t prev = S_NOTDEFINED;
	syn_t expect = S_NOTDEFINED;
	bool unary_been = false;
	int numlevel = level + 2;
	int arithlevel = level + 1;
	int next_del = next_delimiter(num, level, true);
	if(next_del<0) {
		pr_warn("could not find delimiter when processing expression");
		next_del = str_array.amount;
	}
	if(inside_expr) {
		int brace = next_closing_brace(num, level);
		if(brace>=0 && brace < next_del) {
			next_del = brace;
		}
	}
	for(; num<next_del;) {
		switch(str_get(num)->lext) {
		case L_CONSTANT:
		case L_CONSTANT_BIN:
		case L_CONSTANT_FLOAT:
		case L_CONSTANT_HEX:
		case L_CHAR:
			if(prev == S_CONST_PARAM) {
				err_amount++;
				set_synt_err_unexp(num, S_OPERAT_BINARY, S_CONST_PARAM);
				num++;
				break;
			}
			prev = S_CONST_PARAM;
			set_synt(num, S_CONST, numlevel);
			num++;
			break;
		case L_OPERAT_ASSIGNMENT:
			if(prev == S_ID_VARIABLE &&
					(str_get(num+1)->lext == L_CHAR ||
					str_get(num+1)->lext == L_CONSTANT||
					str_get(num+1)->lext == L_CONSTANT_BIN||
					str_get(num+1)->lext == L_CONSTANT_FLOAT||
					str_get(num+1)->lext == L_CONSTANT_HEX||
					str_get(num+1)->lext == L_IDENTIFIER||
					str_get(num+1)->lext == L_OPERAT_ARITHMETIC)
					) {
				set_synt(num, S_OPERAT_ASSIGNMENT, arithlevel);
				num++;
				return process_expression(num, level+1, inside_expr, inside_array);
			} else {
				err_amount++;
				set_synt_err_unexp(num, S_OPERAT_BINARY, S_OPERAT_ASSIGNMENT);
				num++;
				break;
			}
		case L_OPERAT_ARITHMETIC:
		case L_OPERAT_BITWISE:
		case L_OPERAT_RELATION:
		case L_OPERAT_LOGIC:
		{
			const char* unary[] = { "++", "--"};
			const char* sign[] = { "-", "+", "!"};
			const char* binary[] = {
					"-", "+", "*", "/", "%",
					"=", ">","<", "<>", ">=", "<=",
					"not", "or", "xor", "mod", "div",
					"&", "|", "^", "~", "<<", ">>"
			};
			if(is_str_in(str_get(num)->inst, unary, sizeof unary)) {
				pr_debug("unary op detected num=%d", num);
				if(prev == S_ID_VARIABLE || str_get(num+1)->lext == L_IDENTIFIER) {
					prev = S_OPERAT_UNARY;
					expect = S_NOTDEFINED;
					unary_been = true;
					set_synt(num, S_OPERAT_UNARY, numlevel+1);
					num++;
				} else {
					prev = S_OPERAT_UNARY;
					set_synt_err_unexp(num, S_ID_UNDEFINED, S_OPERAT_UNARY);
					str_get(num)->level = numlevel + 1;
					err_amount++;
					num++;
				}
				break;
			}
			if(is_str_in(str_get(num)->inst, sign, sizeof sign)) {
				pr_debug("sign op detected num=%d", num);
				if((str_get(num+1)->lext == L_IDENTIFIER||
						str_get(num+1)->lext == L_CONSTANT ||
						str_get(num+1)->lext == L_CONSTANT_FLOAT ||
						str_get(num+1)->lext == L_CONSTANT_BIN ||
						str_get(num+1)->lext == L_CONSTANT_HEX
						)&&
						str_get(num-1)->lext != L_IDENTIFIER) {
					prev = S_OPERAT_UNARY;
					expect = S_ID_VARIABLE;
					unary_been = true;
					set_synt(num, S_OPERAT_UNARY, numlevel+1);
					num++;
					break;
				}
			}
			if(is_str_in(str_get(num)->inst, binary, sizeof binary)) {
				pr_debug("binary op num=%d", num);
				if((str_get(num+1)->lext == L_IDENTIFIER||
						str_get(num+1)->lext == L_CONSTANT ||
						str_get(num+1)->lext == L_CONSTANT_FLOAT ||
						str_get(num+1)->lext == L_CONSTANT_BIN ||
						str_get(num+1)->lext == L_CONSTANT_HEX
						)&& (
						str_get(num-1)->lext == L_IDENTIFIER||
						str_get(num-1)->lext == L_CONSTANT ||
						str_get(num-1)->lext == L_CONSTANT_FLOAT ||
						str_get(num-1)->lext == L_CONSTANT_BIN ||
						str_get(num-1)->lext == L_CONSTANT_HEX ||
						prev == S_ID_VARIABLE
						)) {
					expect = S_ID_VARIABLE;
					prev = S_OPERAT_BINARY;
					unary_been = true;
					set_synt(num, S_OPERAT_BINARY, arithlevel);
					num++;
					break;
				} else {
					err_amount++;
					set_synt_err_unexp(num, S_CONST, S_OPERAT_BINARY);
					str_get(num)->level = arithlevel;
					num++;
					break;
				}
			}

			break;
		}
		case L_DELIMITER:
			return num;
		case L_KEYWORD:
			return num;
		case L_IDENTIFIER:
			if(prev == S_ID_VARIABLE) {
				err_amount++;
				set_synt_err_unexp(num, S_OPERAT_BINARY, S_ID_VARIABLE);
				num++;
				break;
			}
			num = process_ident(num, numlevel, true, true);
			prev = S_ID_VARIABLE;
			break;
		case L_BRACE_CLOSING:
			if(!strcmp(str_get(num)->inst, "]")&& inside_array) {
				return num;
			}
		default:
			err_amount++;
			pr_err("Unexpected operator");
			str_get(num)->level = 0;
			set_synt_err_unexp(num, S_NOTDEFINED, lex_to_syn(str_get(num)->lext));
			num++;
		}
	}
	return num;
}

int next_delimiter(int num, int level, bool param)
{
	for(int i=num; i<str_array.amount; i++) {
		if(str_get(i)->lext == L_DELIMITER ) {
			if(param && !strcmp(str_get(i)->inst, ",")) {
				return i;
			}
			else if(!strcmp(str_get(i)->inst, ";")) {
				return i;
			}
		}
	}
	return -1;
}

int next_closing_brace(int num, int level)
{
	for(int i=num; i<str_array.amount; i++) {
		if(!strcmp(str_get(i)->inst, ")") ) {
			return i;
		}
	}
	return -1;
}

int process_array(int num, int level)
{
	if(
			str_get(num)->lext == L_IDENTIFIER &&
			!strcmp(str_get(num + 1)->inst, "[")
			) {
		set_synt(num, S_ID_VARIABLE, level);
		set_synt(num+1, S_BRACE_OPEN, level+1);
		num = process_expression(num+2, level+2, true, true);
		if(num>=str_array.amount)
			return num++;
		if( !strcmp(str_get(num)->inst, "]")) {
			set_synt(num, S_BRACE_CLOSE, level+1);
			return num+1;
		} else {
			set_synt_err(num, S_BRACE_CLOSE);
			str_get(num)->level = level+1;
			return num+1;
		}
	} else {
		return ++num;
	}
	return num++;
}

/* for < variable-name > := < initial_value >
 * to [down to] < final_value > do
   S;
 */
int process_for_loop(int num, int level)
{
	/* for */
	if(strcasecmp(str_get(num)->inst, "for")) {
		pr_err("no 'for' inside 'for' loop");
		set_synt_err(num, S_KEYWORD);
		err_amount++;
		return ++num;
	}
	set_synt(num, S_KEYWORD, level);
	num++;
	/* <variable name> */
	if((str_get(num)->lext != L_IDENTIFIER)) {
		set_synt_err(num, S_ID_VARIABLE);
		err_amount++;
		return ++num;
	}
	num = process_ident(num, level+2, true, true);
	/* := */
	if(str_get(num)->lext != L_OPERAT_ASSIGNMENT) {
		set_synt_err(num, S_OPERAT_ASSIGNMENT);
		err_amount++;
		return ++num;
	}
	set_synt(num, S_OPERAT_ASSIGNMENT, level+1);
	num++;
	/* <initial value> */
	num = process_expression(num, level+2, false, false);
	/* to [down to] */
	const char * arr[] = { "to", "downto" };
	if(str_get(num)->lext != L_KEYWORD ||
			(!is_str_in(str_get_inst(num), arr, sizeof arr))) {
		set_synt_err(num, S_KEYWORD);
		err_amount++;
		return ++num;
	}
	set_synt(num++, S_KEYWORD, level+1);
	/* <final value> */
	num = process_ident(num, level+2, false, false);
	/* do */
	if(strcasecmp(str_get_inst(num), "do")) {
		set_synt_err(num, S_KEYWORD);
		err_amount++;
		return ++num;
	}
	set_synt(num++, S_KEYWORD, level+1);
	/* S */
	/* may be block */
	if(!strcasecmp(str_get_inst(num), "begin")) {
		return process_block(num, level+3);
	}
	return process_ident(num, level + 3, false, false);
}

 /* begin <code...;.;.;> end; */
int process_block(int num, int level)
{
	if(strcasecmp(str_get_inst(num), "begin")) {
		/* not block */
		set_synt_err(num, S_KEYWORD);
		return num+1;
	}
	set_synt(num++, S_KEYWORD, level);
	return process_main(num, level+1, true);
}
