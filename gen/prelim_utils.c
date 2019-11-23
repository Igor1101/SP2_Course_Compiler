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
#include <semantic/sem.h>
#include "prelim.h"

static int _add(op_t op, int argc, var_t* a0, var_t* a1);

static int _add(op_t op, int argc, var_t* a0, var_t* a1)
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

ctypes_t str_get_memtype(int num)
{
	switch(str_get(num)->synt) {
	case S_CONST:
	case S_CONST_PARAM:
	case S_CHAR:
		return CONSTANT;
		break;
	case S_ID_ARRAY:
	case S_ID_PARAM:
	case S_ID_UNDEFINED:
	case S_ID_VARIABLE:
	case S_ID_FUNCTION:
	case S_STRING:
		return MEMORY_LOC;
		break;
	default:
		return MEMORY_LOC;
	}
}

int add_bin(op_t op, var_t* var0, var_t* var1)
{
	return _add(op, 2, var0, var1);
}

int add_un(op_t op, var_t*var0)
{
	return _add(op, 1, var0, NULL);
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
	return NULL;
}

void prelim_print_debug(void)
{
	pr_info("Prelim output:");
	for(int i=0; i<pre_code.amount; i++) {
		inst_t* inst = &pre_code.inst[i];
		printf("\t%s\t", op_to_str(inst->opcode));
		switch(inst->argc) {
		case 0:
			puts("");
			break;
		case 1:
			printf("<%s>%s%s", type_to_str(inst->arg0.type),
						var_get_inst(&inst->arg0),
						(inst->arg0.arrayel)?"[]":""
						);
			puts("");
			break;
		case 2:
			printf("<%s>%s%s", type_to_str(inst->arg0.type),
						var_get_inst(&inst->arg0),
						(inst->arg0.arrayel)?"[]":""
						);
			printf(",\t");
			printf("<%s>%s%s", type_to_str(inst->arg1.type),
						var_get_inst(&inst->arg1),
						(inst->arg1.arrayel)?"[]":""
						);
			puts("");
			break;
		}
	}
}

struct regs_vm_t {
	bool reg_res[REGS_AMOUNT];
	var_t regs[REGS_AMOUNT];
}regs_vm;

int reserve_reg(ctypes_t type)
{
	for(int i=0; i<REGS_AMOUNT; i++) {
		if(!regs_vm.reg_res[i]) {
			regs_vm.regs[i].memtype = REGISTER;
			regs_vm.regs[i].type = type;
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

var_t* get_reg(int num)
{
	return &regs_vm.regs[num];
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
	return "invalid reg";
}

void free_ops(void)
{
	for(int i=0; i<pre_code.amount; i++) {
		//if(pre_code.inst[i].argc>=1)
		//	str_free((void**)&pre_code.inst[i].arg0.inst);
		//if(pre_code.inst[i].argc>=2)
		//	str_free((void**)&pre_code.inst[i].arg1.inst);
	}
	str_free((void**)&pre_code.inst);
	pre_code.amount = 0;
}

void init_prelim(void)
{
	free_ops();
}

int var_get(int num, mem_t mem, var_t* var)
{
	if(var == NULL) {
		pr_err("str_get_var NULL pointer");
		return -1;
	}
	var->memtype = mem;
	var->num = num;
	switch(mem) {
	case MEMORY_LOC:
		var->conv = str_get(num)->conv_to;
		var->type = str_get(num)->ctype;
		var->arrayel = str_get(num)->array;
		break;
	case REGISTER:
		*var = *get_reg(num);
		break;
	}
	return 0;
}

char* var_get_inst(var_t *var)
{
	switch(var->memtype) {
	case REGISTER:
		return reg_to_str(var->num);
	case MEMORY_LOC:
		return str_get(var->num)->inst;
	default:
		return NULL;
	}
}
