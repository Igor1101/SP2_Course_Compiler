/*
 * sem_c.c
 *
 *  Created on: Nov 5, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <defs.h>
#include <lexems/tables.h>
#include "sem.h"
#include "tables_sem.h"

static int process_declaration(int num);
static int process_expression(int num, bool param, bool inside_array);
static int process_array(int num, bool param);
static int prev_var_expr(int start, int end);
static int next_var_expr(int start, int end);

int sem_analyze(void)
{
	ident_array_remove();
	for(int i = 0; i<str_array.amount; i++) {
		str_get(i)->sem_err = false;
	}
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_KEYWORD_TYPE:
			num = process_declaration(num);
			break;
		case S_ID_VARIABLE:
		case S_OPERAT_UNARY:
		case S_ID_ARRAY:
			num = process_expression(num, false, false);
			break;
		default:
			num++;
		}
	}
	sem_database_print();
	return sem_err_amount();
}

static int process_declaration(int num)
{
	/* getting type of variables */
	ctypes_t t = str_to_type(str_get(num)->inst);
	/* getting declared vars */
	for(; num<next_delimiter(num, 0, false); ) {
		switch(str_get(num)->synt) {
		case S_ID_VARIABLE:
		{
			int idnum = ident_add(str_get_inst(num), t, false, 1);
			if(idnum<0) {
				set_err_already_decl(num);
			}
			str_get(num)->ctype = ident_get(idnum)->type;
			num++;
			if(str_get(num)->synt == S_OPERAT_ASSIGNMENT) {
				num = process_expression(num-1, true, false);
			}
			break;
		}
		case S_ID_ARRAY:
			if(ident_add(str_get_inst(num), t, true,
					strtol(str_get(num+2)->inst, NULL, 10))<0) {
				set_err_already_decl(num);
			}
			str_get(num)->ctype = t;
			num = process_array(num, false);
			break;

		default:
			num++;
		}
	}
	return num;
}

static int process_expression(int num, bool param, bool inside_array)
{
	int start = num;
	int end = next_delimiter(num, 0, param);
	bool assign_used = false;
	int assigned_var;
	ctypes_t main_type = C_UKNOWN;
	for(; num<end;) {
		switch(str_get(num)->synt) {
		case S_OPERAT_BINARY:
			if(str_get(num)->lext== L_OPERAT_RELATION) {
				int prev = prev_var_expr(start, num);
				int next = next_var_expr(num, end);
				if(ident_present(str_get(prev)->inst) &&
						ident_present(str_get(next)->inst)) {
				if(ident_get_str(str_get(prev)->inst)->type !=
						ident_get_str(str_get(next)->inst)->type
						) {
					set_err(num, "relation with different types");
				}
				}
			}
			num++;
			break;
		case S_CONST:
			if(str_get(num)->lext == L_CONSTANT) {
				str_get(num)->ctype = C_INT_T;
				if(main_type == C_UKNOWN)
					main_type = C_INT_T;
				else if(main_type == C_INT_T ||
						main_type == C_SHORT_T ||
						main_type == C_LONG_T ||
						main_type == C_CHAR_T)
					str_get(num)->ctype = main_type;
			}

			if(str_get(num)->lext == L_CONSTANT_FLOAT) {
				str_get(num)->ctype = C_FLOAT_T;
				if(main_type != C_UKNOWN) {
					if(main_type != C_FLOAT_T && main_type != C_DOUBLE_T)
						set_err_type(num, C_FLOAT_T, main_type);
				} else {
					main_type = C_DOUBLE_T;
				}
			}
			{
				ctypes_t t = str_get(num)->ctype;
				if(main_type != C_UKNOWN) {
				switch(type0_to_type1_acc(t, main_type)) {
				case ACCEPT:
					break;
				case NOTACCEPT:
					set_err_type(num, t, main_type);
					break;
				case CONVERT:
					str_get(num)->conv_to = main_type;
					break;
				}
				}
			}
			num++;
			break;
		case S_BRACE_CLOSE:
			if(inside_array)
				return num;
			num++;
			break;
		case S_ID_ARRAY:
			if(!ident_present(str_get(num)->inst)) {
				set_err_undeclared_used(num);
				num++;
				break;
			}
			str_get(num)->ctype = ident_get_str(str_get_inst(num))->type;
			ctypes_t t = ident_get_str(str_get_inst(num))->type;
			if(inside_array) {
				if(t == C_DOUBLE_T || t == C_FLOAT_T) {
					set_err(num, "type <%s> inside array elem", t);
					num++;
					break;
				}
			}
			if(main_type != C_UKNOWN) {
				switch(type0_to_type1_acc(t, main_type)) {
				case ACCEPT:
					break;
				case NOTACCEPT:
					set_err_type(num, t, main_type);
					break;
				case CONVERT:
					str_get(num)->conv_to = main_type;
					break;
				}
			} else {
				main_type = t;
			}
			num = process_array(num, param);
			break;
		case S_ID_VARIABLE:
		{
			if(!ident_present(str_get(num)->inst)) {
				set_err_undeclared_used(num);
				num++;
				break;
			}
			str_get(num)->ctype = ident_get_str(str_get_inst(num))->type;
			ctypes_t t = ident_get_str(str_get_inst(num))->type;
			if(inside_array) {
				if(t == C_DOUBLE_T || t == C_FLOAT_T) {
					set_err(num, "type <%s> inside array elem", type_to_str(t));
					num++;
					break;
				}
			}
			if(main_type != C_UKNOWN) {
				switch(type0_to_type1_acc(t, main_type)) {
				case ACCEPT:
					break;
				case NOTACCEPT:
					set_err_type(num, t, main_type);
					break;
				case CONVERT:
					str_get(num)->conv_to = main_type;
					break;
				}
			} else {
				main_type = t;
			}
			ident_set_used(str_get_inst(num));
			if(!strcmp(str_get(num+1)->inst, "=")) {
				ident_set_init(str_get_inst(num));
				assign_used = true;
				assigned_var = num;
				main_type = ident_get_str(str_get_inst(num))->type;
				num++;
				break;
			}
			if(str_get(num+1)->synt == S_OPERAT_BINARY ||
					str_get(num-1)->synt == S_OPERAT_BINARY
					) {
				ident_set_used(str_get_inst(num));
				num++;
				break;
			}
			if((str_get(num+1)->synt == S_OPERAT_UNARY &&
					(!strcmp(str_get(num+1)->inst, "--")||
					!strcmp(str_get(num+1)->inst, "++"))
					)||
					(str_get(num-1)->synt == S_OPERAT_UNARY &&
					(!strcmp(str_get(num-1)->inst, "--")||
					!strcmp(str_get(num-1)->inst, "++")))
							) {
				ctypes_t t = ident_get_str(str_get_inst(num))->type;
				if( t == C_FLOAT_T || t == C_DOUBLE_T) {
					set_err(num, "for type <%s> op not acceptable", type_to_str(t));
					num++;
					break;
				}
			}
			ident_set_used(str_get_inst(num));
			num++;
			break;
		}
		break;
		default:
			num++;
		}
	}
	return num;
}

static int process_array(int num, bool param)
{
	str_get(num)->array = true;
	/* we are at the beginning */
	num += 2;
	/* at the expr */
	num = process_expression(num, param, true);
	/* at ] */
	return num+1;
}

static int prev_var_expr(int start, int end)
{
	int num = end;
	do {
		num--;
		/* if array found */
		if(str_get(num)->synt == S_BRACE_CLOSE) {
			while(str_get(num)->synt != S_BRACE_OPEN)
				num--;
		}
		if(str_get(num)->synt == S_ID_VARIABLE
				|| str_get(num)->synt == S_CONST ||
				str_get(num)->synt == S_ID_ARRAY)
			return num;
	} while(num>=start);
	return num;
}

static int next_var_expr(int start, int end)
{
	int num = start;
	do {
		num++;
		/* if array found */
		if(str_get(num)->synt == S_BRACE_OPEN) {
			while(str_get(num)->synt != S_BRACE_CLOSE)
				num++;
		}
		if(str_get(num)->synt == S_ID_VARIABLE
				|| str_get(num)->synt == S_CONST ||
				str_get(num)->synt == S_ID_ARRAY)
			return num;
	} while(num<end);
	return num;
}

