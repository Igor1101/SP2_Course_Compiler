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
		case S_ID_VARIABLE:
		{
			if(!ident_present(str_get(num)->inst)) {
				set_err_undeclared_used(num);
				num++;
				break;
			}
			if(!strcmp(str_get(num+1)->inst, "=")) {
				ident_set_init(str_get_inst(num));
				assign_used = true;
				assigned_var = num;
				main_type = ident_get_str(str_get_inst(num))->type;
				num++;
				break;
			}
			if(str_get(num+1)->synt == S_OPERAT_BINARY) {
				ident_set_used(str_get_inst(num));
				num++;
				break;
			}
			if(str_get(num+1)->synt == S_OPERAT_UNARY) {
				ctypes_t t = ident_get_str(str_get_inst(index))->type;
				if( t == C_FLOAT_T || t == C_DOUBLE_T) {
					set_err(num, "for type <%s> op not acceptable", type_to_str(t));
				}
			}
		}
		break;
		default:
			num++;
		}
	}
	return num;
}
