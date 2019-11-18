/*
 * prelim.c
 *
 *  Created on: Nov 16, 2019
 *      Author: igor
 */

#include <defs.h>
#include <lexems/tables.h>
#include <semantic/tables_sem.h>
#include "prelim.h"

code_t pre_code = {
		.amount = 0
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
	int savenum = num;
	/* get all vars */
	for(; num<next_delimiter(num, 0, param);num++) {
		if(str_get(num)->synt == S_ID_VARIABLE) {
			prev->varnum = num;
			prev->next = calloc(1, sizeof (struct var_node));
			prev = prev->next;
			prev->next = NULL;
			prev->varnum = -1;
		}
	}
	num = savenum;
	/* unary op first ++<var> (changeable) */
	for(;num<next_delimiter(num, 0, param);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
				if(!strcmp(str_get(num)->inst, "++"))
					add_un(INC, var, NULL);
				if(!strcmp(str_get(num)->inst, "--"))
					add_un(DEC, var, NULL);
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	num = savenum;
	/* unary op  (unchangeable) */
	for(;num<next_delimiter(num, 0, false);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
					/*ignore + */
				if(!strcmp(str_get(num)->inst, "-")) {
					int reg = reserve_reg();
					add_bin(SIGN, reg, reg_to_str(reg), num, NULL);
				}
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	free_vars();
	return num;
}

