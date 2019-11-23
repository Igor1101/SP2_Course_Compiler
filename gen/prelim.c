/*
 * prelim.c
 *
 *  Created on: Nov 16, 2019
 *      Author: igor
 */

#include <defs.h>
#include <lexems/tables.h>
#include <semantic/tables_sem.h>
#include <string.h>
#include "prelim.h"

code_t pre_code = {
		.amount = 0,
		.inst = NULL
};
static int process_expression(int num, bool param);

int preliminary_gen(void)
{
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_OPERAT_ASSIGNMENT:
		{
			pr_debug("assignment detected");
			num = process_expression(num-1, false);
			break;
		}
		case S_ID_ARRAY:
		case S_ID_FUNCTION:
		case S_ID_PARAM:
		case S_ID_VARIABLE:
		case S_OPERAT_BINARY:
		case S_OPERAT_UNARY:
		{
			pr_debug("expr detected");
			num = process_expression(num, false);
			break;
		}
		default:
			num++;
		}
	}
	return 0;
}

struct var_node{
	int varnum;
	bool in_reg;
	int reg;
	struct var_node* next;
};
static int process_expression(int num, bool param)
{
	int savenum = num;
	int rvalue_reg = -1;
	int rvalue_num = -1;
	struct var_node* var0 = calloc(1, sizeof(struct var_node));
	struct var_node* prev = var0;
	void free_vars(void)
	{
		struct var_node*i = var0;
		while(i->next != NULL) {
			struct var_node*pr = i;
			i = i->next;
			free(pr);
		}
		free(i);
	}
	int set_var_reg(int reg, int varnum)
	{
		struct var_node*i = var0;
		do {
			if(i->varnum == varnum && i->reg == -1) {
				i->reg = reg;
				return 0;
			}
			i = i->next;
		} while(i != NULL);
		return -1;
	}
	void get_vars(void)
	{
		for(num=savenum; num<next_delimiter(num, 0, param);num++) {
			if(str_get(num)->synt == S_ID_VARIABLE) {
				prev->varnum = num;
				prev->next = calloc(1, sizeof (struct var_node));
				prev = prev->next;
				prev->next = NULL;
				prev->varnum = -1;
				prev->reg = -1;
			}
		}
	}
	/* get all vars */
	get_vars();
	/* unary op first ++<var> (changeable) */
	for(num=savenum;num<next_delimiter(num, 0, param);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
				if(!strcmp(str_get(num)->inst, "++")) {
					var_t v;
					var_get(var, MEMORY_LOC, &v);
					add_un(INC, &v);
				}
				if(!strcmp(str_get(num)->inst, "--")) {
					var_t v;
					var_get(var, MEMORY_LOC, &v);
					add_un(DEC, &v);
				}
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	/* unary op  (unchangeable) */
	for(num=savenum;num<next_delimiter(num, 0, false);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
					/*ignore + */
				if(!strcmp(str_get(num)->inst, "-")) {
					int reg = reserve_reg(str_get(var)->ctype);
					rvalue_reg = reg;
					var_t from, to;
					var_get(var, MEMORY_LOC, &from);
					var_get(reg, REGISTER, &to);
					add_bin(SIGN, &to, &from);
					set_var_reg(reg, var);
				}
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	/* assignment op */
	for(num=savenum;num<next_delimiter(num, 0, false);num++) {
		if(str_get(num)->synt == S_OPERAT_ASSIGNMENT) {
			int result = num - 1;
			var_t from, to;
			var_get(result, MEMORY_LOC, &to);
			var_get(rvalue_reg, REGISTER, &from);
			add_bin(MOV, &to, &from);
		}
	}
	free_vars();
	return num;
}

