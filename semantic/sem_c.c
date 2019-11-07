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

int sem_analyze(void)
{
	ident_array_remove();
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
			if(ident_add(str_get_inst(num), t, false)<0) {
				set_err_already_decl(num);
			}
			if(str_get(num+1)->synt == S_OPERAT_ASSIGNMENT) {
				num = process_expression(num, true, false);
			} else {
			}
			num++;
			break;
		case S_ID_ARRAY:
			if(ident_add(str_get_inst(num), t, true)<0) {
				set_err_already_decl(num);
			}
			if(str_get(num+1)->synt == S_OPERAT_ASSIGNMENT) {
				num = process_expression(num, true, false);
			} else {
			}
			num++;
			break;

		default:
			num++;
		}
	}
	return num;
}

static int process_expression(int num, bool param, bool inside_array)
{
	bool assign_used = false;
	int assigned_var;
	ctypes_t main_type = C_UKNOWN;
	for(; num<next_delimiter(num, 0, param);) {
		switch(str_get(num)->synt) {
		case S_BRACE_CLOSE:
			if(inside_array)
				return num;
			num++;
			break;
		case S_ID_VARIABLE:
		{
			if(!ident_present(str_get(num)->inst)) {
				set_err_undeclared_used(num);
				num++;
				break;
			}
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
			if(str_get(num+1)->synt == S_OPERAT_UNARY ||
					str_get(num-1)->synt == S_OPERAT_UNARY) {
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

int process_array(int num, bool param)
{
	/* we are at the beginning */
	num += 2;
	/* at the expr */
	num = process_expression(num, param, true);
	/* at ] */
	return num+1;
}
