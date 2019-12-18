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

int err_amount = 0;
int parensis_nest = 0;
int curlybrace_nest = 0;
int array_nest = 0;

static int process_ident(int num, int level, bool maybeparam, bool inside_expr);
static int process_expression(int num, int level, bool inside_expr, bool inside_array);
static int process_function(int num, int level);
static int process_for_loop(int num, int level);
static int process_main(int num, int level, bool inside_block);
static int process_block(int num, int level);
static int process_declaration(int num, int level, bool param);
static int process_array(int num, int level, bool maybeparam, bool inside_expr);
static int process_parensis(int num, int level, bool maybeparam, bool inside_expr);

int syn_analyze(void)
{
	init_syn_analyzer();
	process_main(0, 0, false);
	return syn_err_amount();
}

static int process_main(int num, int level, bool inside_block)
{
	for(num=0; num<str_array.amount; ) {
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
		case L_BRACE_OPENING:
			num = process_parensis(num, level, inside_block, false);
			break;
		case L_IDENTIFIER:
			num = process_ident(num, level+1, false, false);
			break;
		case L_OPERAT_ARITHMETIC:
			num = process_expression(num, level+1, false, false);
			break;
		case L_BRACE_CLOSING:
			//close_brace(num);
		case L_DELIMITER:
			num = is_delimiter_next_expected(num, level, true);
			break;
		case L_KEYWORD:
		{
			const static char* types[] = { "int", "float",
			"char", "double", "long", "short"};
			if(is_str_in(str_get(num)->inst, types, sizeof types)) {
				num = process_declaration(num, level, false);
				break;
			}
		}
		default:
			pr_err("UNEXPECTED SYMBOL");
			set_synt_err_unexp(num, S_NOTDEFINED, lex_to_syn(str_get(num)->lext));
			num++;
		}
		if(num >= str_array.amount) {
			if(str_array.amount>0 && (str_get(str_array.amount - 1)->lext != L_DELIMITER)) {
				set_synt_err(str_array.amount - 1, S_DEL);
			}
			return num;
		}
		int next = num;
		num = is_delimiter_next_expected(num, level, next == prev);
    }
	if(str_array.amount>0 && (str_get(str_array.amount - 1)->lext != L_DELIMITER)) {
		set_synt_err(str_array.amount - 1, S_DEL);
	}
	if(inside_block)
		end_set_synt_err_inst(S_BRACE_CLOSE, "}");
	return num;
}


int close_brace(int num)
{
	int ret;
	switch(*(str_get(num)->inst)) {
	case ')':
		ret = --parensis_nest;
		break;
	case '}':
		ret = --curlybrace_nest;
		break;
	case ']':
		ret = --array_nest;
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
		ret = ++parensis_nest;
		break;
	case '{':
		ret = ++curlybrace_nest;
		break;
	case '[':
		ret = ++array_nest;
		break;
	default:
		pr_warn("open_brace garbage detected %c", *(str_get(num)->inst));
	}
	return ret;
}

static int process_ident(int num, int level, bool maybeparam, bool inside_expr)
{
	if(num < (str_array.amount-1)) {

		if(str_get(num)->lext == L_IDENTIFIER &&
			!strcmp(str_get(num + 1)->inst, "[")) {
			int i = process_array(num, level, maybeparam, inside_expr);
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
				return process_expression(num, level+1, maybeparam, false);
			}
		}
		if(maybeparam && (str_get(num+1)->lext == L_BRACE_CLOSING ||
				str_get(num+1)->lext == L_DELIMITER)) {
			set_synt(num, S_ID_VARIABLE, level);
			return ++num;
		}
	}
	if(num > 0) {
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
	set_synt_err_unexp(num, S_ID_VARIABLE, S_ID_UNDEFINED);
	return ++num;
}

static int process_function(int num, int level)
{
	bool has_del = false;
	int level_del = level+3;
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
		pr_warn("could not found next delimiter");
		next_del = str_array.amount;
	}
	syn_t prev = S_FUNC_BRACE_OPEN;
	for(int i=num+2; i<next_del; ) {
		switch(str_get(i)->lext) {
		case L_BRACE_CLOSING:
			if(!strcmp(str_get(i)->inst, ")") ) {
				pr_debug("closing func brace");
				set_synt(i, S_FUNC_BRACE_CLOSE, level+2);
				return i+1;
			}
			/* unexpected brace */
			set_synt_err(i, S_FUNC_BRACE_CLOSE);

			return i+1;
		case L_IDENTIFIER:
		{
			int next = process_ident(i, level_param, true, false);
			if(str_get(i)->synt == S_ID_PARAM) {
				pr_debug("it was param");
			}
			i = next;
			prev = S_ID_VARIABLE;
			break;
		}
		case L_CHAR:
		case L_CONSTANT:
		case L_CONSTANT_BIN:
		case L_CONSTANT_FLOAT:
		case L_CONSTANT_HEX:
		case L_STRING:
			i = process_expression(i, level_param, true, true);
			prev = S_CONST_PARAM;
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
				if(prev == S_DEL_PARAM || prev == S_FUNC_BRACE_OPEN) {
					set_synt_err_unexp(i, S_ID_PARAM, S_DEL_PARAM);
				} else {
					set_synt(i, S_DEL_PARAM, level_del);
				}
				prev = S_DEL_PARAM;
				i++;
				break;
			}
			set_synt_err_unexp(i, S_FUNC_BRACE_CLOSE, S_DEL_PARAM);
			i++;
			break;
		default:
			set_synt_err_unexp(i, S_ID_PARAM, lex_to_syn(str_get(i)->lext));
			i++;
		}
	}
	set_synt_err(next_del, S_BRACE_CLOSE);
	pr_err("Expected ')' brace at end of input");
	return str_array.amount;
}

static int process_expression(int num, int level, bool inside_expr, bool inside_array)
{
	syn_t prev = S_NOTDEFINED;
	syn_t expect = S_NOTDEFINED;
	bool unary_been = false;
	/* operator precedence */
	int numlevel = level + 5;
	int arithlevel0 = level + 4;
	int arithlevel1 = level + 3;
	int arithlevel2 = level + 2;
	int arithlevel3 = level + 1;
	int arithlevel = level + 1;
	int next_del = next_delimiter(num, level, true);
	if(next_del<0) {
		pr_warn("could not find delimiter when processing expression");
		next_del = str_array.amount;
	}
	/*
	if(inside_expr) {
		int brace = next_closing_brace(num, level);
		if(brace>=0 && brace < next_del) {
			next_del = brace;
		}
	}*/
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
					(
					/* op inside parensis */
					!strcmp(str_get(num+1)->inst, "(") ||
					str_get(num+1)->lext == L_CHAR ||
					str_get(num+1)->lext == L_CONSTANT||
					str_get(num+1)->lext == L_CONSTANT_BIN||
					str_get(num+1)->lext == L_CONSTANT_FLOAT||
					str_get(num+1)->lext == L_CONSTANT_HEX||
					str_get(num+1)->lext == L_IDENTIFIER||
					str_get(num+1)->lext == L_OPERAT_ARITHMETIC) &&
					(str_get(num-2)->synt != S_OPERAT_BINARY)
					) {
				set_synt(num, S_OPERAT_ASSIGNMENT, arithlevel);
				num++;
				if(!strcmp(str_get(num)->inst, "(")) {
					prev = S_ID_VARIABLE;
					num = process_parensis(num, level+5, false, true);
					break;
				}
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
			/* operator precedence */
			const char* unary[] = { "++", "--"};
			const char* sign[] = { "-", "+", "!"};
			const char* binary[] = {
					"-", "+", "*", "/", "%",
					"==", ">","<", "!=", ">=", "<=",
					"&&", "||",
					"&", "|", "^", "~", "<<", ">>"
			};
			const char* bin_prio0[] = {
					 "*", "/", "%"
			};
			const char* bin_prio1[] = {
					 "+", "-"
			};
			const char* bin_prio2[] = {
					 "<<", ">>"
			};
			const char* bin_prio3[] = {
					 "<", ">", "<=", ">=", "==", "!="
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
						str_get(num-1)->lext != L_IDENTIFIER  &&
						str_get(num-1)->lext != L_CONSTANT &&
						prev != S_ID_VARIABLE) {
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
						str_get(num+1)->lext == L_CONSTANT_HEX ||
						!strcmp(str_get(num+1)->inst, "(") ||
						!strcmp(str_get(num+1)->inst, "+") ||
						!strcmp(str_get(num+1)->inst, "-")
						)&& (
						str_get(num-1)->lext == L_IDENTIFIER||
						str_get(num-1)->lext == L_CONSTANT ||
						str_get(num-1)->lext == L_CONSTANT_FLOAT ||
						str_get(num-1)->lext == L_CONSTANT_BIN ||
						str_get(num-1)->lext == L_CONSTANT_HEX ||
						!strcmp(str_get(num-1)->inst, ")") ||
						prev == S_ID_VARIABLE
						)) {
					expect = S_ID_VARIABLE;
					prev = S_OPERAT_BINARY;
					unary_been = true;
					if(is_str_in(str_get(num)->inst, bin_prio0, sizeof bin_prio0))
						set_synt(num, S_OPERAT_BINARY, arithlevel0);
					else if(is_str_in(str_get(num)->inst, bin_prio1, sizeof bin_prio1))
						set_synt(num, S_OPERAT_BINARY, arithlevel1);
					else if(is_str_in(str_get(num)->inst, bin_prio2, sizeof bin_prio2))
						set_synt(num, S_OPERAT_BINARY, arithlevel2);
					else if(is_str_in(str_get(num)->inst, bin_prio3, sizeof bin_prio3))
						set_synt(num, S_OPERAT_BINARY, arithlevel3);
					else
						set_synt(num, S_OPERAT_BINARY, arithlevel);
					num++;
					break;
				} else {
					set_synt_err_unexp(num, S_CONST, S_OPERAT_BINARY);
					num++;
					break;
				}
			}

			break;
		}
		case L_DELIMITER:
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
			} else if(!strcmp(str_get(num)->inst, ")")&& inside_expr) {
				return num;
			}
			set_synt_err_unexp(num, S_NOTDEFINED, S_BRACE_CLOSE);
			num++;
			break;
		case L_BRACE_OPENING:
			if(!strcmp(str_get(num)->inst, "(")) {
				num = process_parensis(num, level, false, inside_expr);
				break;
			}
		default:
			pr_err("Unexpected operator");
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

static int process_array(int num, int level, bool maybeparam, bool inside_expr)
{
	if(
			str_get(num)->lext == L_IDENTIFIER &&
			!strcmp(str_get(num + 1)->inst, "[")
			) {
		set_synt(num, S_ID_ARRAY, level);
		set_synt(num+1, S_BRACE_OPEN, level+1);
		num = process_expression(num+2, level+2, true, true);
		if(num>=str_array.amount)
			return num++;
		if( !strcmp(str_get(num)->inst, "]")) {
			set_synt(num, S_BRACE_CLOSE, level+1);
			if(str_get(num+1)->lext == L_OPERAT_ASSIGNMENT && !inside_expr) {
				set_synt(num+1, S_OPERAT_ASSIGNMENT, level);
				return process_expression(num+2, level, inside_expr, false);
			}
			return num+1;
		} else {
			set_synt_err(num, S_BRACE_CLOSE);
			return num+1;
		}
	} else {
		return ++num;
	}
	return num++;
}

static int process_declaration(int num, int level, bool param)
{
	const static char* types[] = { "int", "float",
			"char", "double", "long", "unsigned", "short"};
	if(!is_str_in(str_get_inst(num), types, sizeof types)) {
		set_synt_err(num, S_KEYWORD_TYPE);
		return num+1;
	}
	set_synt(num, S_KEYWORD_TYPE, level);
	num++;
	if(str_get(num)->lext != L_IDENTIFIER) {
		set_synt_err(num, S_ID_VARIABLE);
		return num+1;
	}
	int nxt_del;
	nxt_del = next_delimiter(num, level, param);
	if(nxt_del < 0) {
		pr_warn("could not find delimiter");
		nxt_del = str_array.amount;
	}
	syn_t prev = S_NOTDEFINED;
	for(; num<nxt_del; ) {
		switch(str_get(num)->lext) {
		case L_IDENTIFIER:
			if(str_get(num+1)->lext == L_OPERAT_ASSIGNMENT) {
				if(strcmp(str_get(num+1)->inst, "=")) {
					int nnext = num+1;
					num = process_ident(num, level+2, false, false);
					set_synt_err_inst(nnext, S_OPERAT_ASSIGNMENT, "=");
					num = nnext+1;
					break;
				}
			} else if(str_get(num+1)->lext != L_DELIMITER &&
				str_get(num+1)->lext != L_BRACE_OPENING) {
				int nnext = num+1;
				num = process_ident(num, level+2, false, false);
				set_synt_err_inst(nnext, S_OPERAT_ASSIGNMENT, "=");
				num = nnext+1;
				break;
			}
			num = process_ident(num, level+2, true, false);
			prev = S_ID_VARIABLE;
			break;
		case L_BRACE_CLOSING:
			return num;
		case L_DELIMITER:
			if(prev == S_DEL) {
				set_synt_err_unexp(num, S_ID_VARIABLE, S_DEL);
				num++;
				break;
			} else {
				set_synt(num, S_DEL, level+1);
				prev = S_DEL;
				num++;
				break;
			}
		default:
			set_synt_err(num, S_DEL);
			return ++num;
		}
	}
	return num;
}

static int process_parensis(int num, int level, bool maybeparam, bool inside_expr)
{
	if( !strcmp(str_get(num)->inst, "(")
			) {
		set_synt(num, S_PARENSIS_OPEN, level);
		num = process_expression(num+1, level+5, true, true);
		if(num>=str_array.amount)
			return num++;
		if( !strcmp(str_get(num)->inst, ")")) {
			set_synt(num, S_PARENSIS_CLOSE, level);
			if(str_get(num+1)->lext == L_OPERAT_ASSIGNMENT && !inside_expr) {
				set_synt(num+1, S_OPERAT_ASSIGNMENT, level);
				return process_expression(num+2, level+1, inside_expr, false);
			}
			return num+1;
		} else {
			set_synt_err(num, S_PARENSIS_CLOSE);
			return num+1;
		}
	} else {
		return ++num;
	}
	return num++;
}

