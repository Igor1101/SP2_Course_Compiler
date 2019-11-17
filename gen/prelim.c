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
static int process_expression(int num);

int preliminary_gen(void)
{
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_OPERAT_ASSIGNMENT:
		{
			pr_debug("assignment detected");
			num = process_expression(num-1);
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
			num = process_expression(num);
			break;
		}
		default:
			num++;
		}
	}
	return 0;
}

static int process_expression(int num)
{
	/* unary op first ++<var> (changeable) */
	for(;num<next_delimiter(num, 0, false);) {
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
	return num;
}

