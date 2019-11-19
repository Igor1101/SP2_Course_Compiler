/*
 * prelim_utils.c
 *
 *  Created on: Nov 17, 2019
 *      Author: igor
 */
#include <defs.h>
#include <stdlib.h>
#include <syntax/syntax.h>
#include <lexems/tables.h>
#include "prelim.h"

static int _add(op_t op, int argc, arg_t* a0, arg_t* a1);

static int _add(op_t op, int argc, arg_t* a0, arg_t* a1)
{
	int strindex = pre_code.amount;
	if(pre_code.inst == NULL) {
		pre_code.inst = (inst_t*)calloc(1,sizeof(inst_t));
		pre_code.amount = 1;
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
	if(a0 != NULL && argc >= 1)
		pre_code.inst[strindex].arg0 = *a0;
	if(a1 != NULL && argc >= 2)
		pre_code.inst[strindex].arg1 = *a1;
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
		default:
			t0 = MEMORY_LOC;
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
		default:
			t1 = MEMORY_LOC;
		}
	} else
		t1 = REGISTER;
	a0.type = t0;
	a1.type = t1;
	a0.inst = (t0 == REGISTER)?str_alloc(num0_reg):str_alloc(str_get_inst(num0));
	a1.inst = (t1 == REGISTER)?str_alloc(num1_reg):str_alloc(str_get_inst(num1));
	return _add(op, 2, &a0, &a1);
}

int add_un(op_t op, int num0, char *num0_reg)
{
	arg_t a0;
	_argt_t t0;
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
		default:
			t0 = MEMORY_LOC;
		}
	} else
		t0 = REGISTER;
	a0.type = t0;
	a0.inst = (t0 == REGISTER)?str_alloc(num0_reg):str_alloc(str_get_inst(num0));
	return _add(op, 1, &a0, NULL);
}

int add_noarg(op_t op)
{
	return _add(op, 0, NULL, NULL);
}

char* op_to_str(op_t op)
{
	switch(op) {
	case NOP:
		return "NOP";
	case INC:
		return "INC";
	case DEC:
		return "DEC";
	case ADD:
		return "ADD";
	case ADDS:
		return "ADDS";
	case SUB:
		return "SUB";
	case SUBS:
		return "SUBS";
	case AND:
		return "AND";
	case OR:
		return "OR";
	case XOR:
		return "XOR";
	case CALL:
		return "CALL";
	case PUSH:
		return "PUSH";
	case POP:
		return "POP";
	case MOV:
		return "MOV";
	case RET:
		return "RET";
	case SHR:
		return "SHR";
	case SHL:
		return "SHL";
	case NOR:
		return "NOR";
	case NOT:
		return "NOT";
	case SIGN:
		return "SIGN";
	case JMP:
		return "JMP";
	case JZ:
		return "JZ";
	case CONV:
		return "CONV";
	}
}

void prelim_print_debug(void)
{
	pr_info("Prelim output:");
	for(int i=0; i<pre_code.amount; i++) {
		inst_t* inst = &pre_code.inst[i];
		pr_info("<%s\t%s, %s>", op_to_str(inst->opcode),
				(inst->argc>0)?inst->arg0.inst:"",
						(inst->argc>1)?inst->arg1.inst:""
		);
	}
}

struct regs_vm_t {
	bool reg_res[REGS_AMOUNT];
}regs_vm;

int reserve_reg(void)
{
	for(int i=0; i<REGS_AMOUNT; i++) {
		if(!regs_vm.reg_res[i]) {
			regs_vm.reg_res[i] = true;
			return i;
		}
	}
	return -1;
}

void free_reg(int r)
{
	 if(r>=REGS_AMOUNT) {
		 pr_err("free reg out of region");
		 return;
	 }
	 regs_vm.reg_res[r] = false;
}

char* reg_to_str(int r)
{
	switch(r) {
	case 0:
		return "r8";
	case 1:
		return "r9";
	case 2:
		return "r10";
	case 3:
		return "r11";
	case 4:
		return "r12";
	case 5:
		return "r13";
	case 6:
		return "r14";
	case 7:
		return "r15";
	case 8:
		return "rax";
	case 9:
		return "rbx";
	case 10:
		return "rcx";
	case 11:
		return "rdx";
	case 12:
		return "rbp";
	case 13:
		return "rdx";
	}
}

void free_ops(void)
{
	for(int i=0; i<pre_code.amount; i++) {
		if(pre_code.inst[i].argc>=1)
			str_free((void**)&pre_code.inst[i].arg0.inst);
		if(pre_code.inst[i].argc>=2)
			str_free((void**)&pre_code.inst[i].arg1.inst);
	}
	str_free((void**)&pre_code.inst);
	pre_code.amount = 0;
}

void init_prelim(void)
{
	free_ops();
}
