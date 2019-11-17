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

int preliminary_gen(void)
{
	for(int num=0; num<str_array.amount;) {

	}
}

static int _add(op_t op, int argc, arg_t a0, arg_t a1)
{
	int strindex = pre_code.amount;
	if(pre_code.inst == NULL) {
		pre_code.inst = (inst_t*)calloc(1,sizeof(inst_t));
		ident_array.amount = 1;
	} else {
		pre_code.amount++;
		pre_code.inst =
				reallocarray(pre_code.inst, pre_code.amount, sizeof(inst_t));
		if(pre_code.inst == NULL) {
			pr_err("array alloc failed");
			exit(-1);
		}
	}
	pre_code.inst[strindex].opcode = op;
	pre_code.inst[strindex].argc = argc;
	pre_code.inst[strindex].arg0 = a0;
	pre_code.inst[strindex].arg1 = a1;
	/* return index */
	return strindex;
}

int add_bin(op_t op, int num0, char *num0_reg, int num1, char* num1_reg)
{
	arg_t a0 , a1;
	_argt_t t0, t1;
	if(num0_reg == NULL) {
		switch(str_get(num0)->synt) {
		case S_CONST:
		case S_CONST_PARAM:
		case S_CHAR:
			t0 = CONSTANT;
			break;
		case S_ID_ARRAY:
		case S_ID_PARAM:
		case S_ID_UNDEFINED:
		case S_ID_VARIABLE:
		case S_ID_FUNCTION:
		case S_STRING:
			t0 = MEMORY_LOC;
			break;
		}
	} else
		t0 = REGISTER;
	if(num1_reg == NULL) {
		switch(str_get(num0)->synt) {
		case S_CONST:
		case S_CONST_PARAM:
		case S_CHAR:
			t1 = CONSTANT;
			break;
		case S_ID_ARRAY:
		case S_ID_PARAM:
		case S_ID_UNDEFINED:
		case S_ID_VARIABLE:
		case S_ID_FUNCTION:
		case S_STRING:
			t1 = MEMORY_LOC;
			break;
		}
	} else
		t1 = REGISTER;
	a0.type = t0;
	a1.type = t1;
	a0.inst = (t0 == REGISTER)?str_alloc(num0_reg):str_alloc(str_get_inst(num0));
	a1.inst = (t1 == REGISTER)?str_alloc(num1_reg):str_alloc(str_get_inst(num1));
	_add(op, 2, a0, a1);
}
