/*
 * prelim.c
 *
 *  Created on: Nov 16, 2019
 *      Author: igor
 */

#include <defs.h>
#include <lexems/tables.h>
#include <semantic/tables_sem.h>
#include <semantic/sem.h>
#include <string.h>
#include "vm.h"
#include "prelim.h"

code_t pre_code = {
		.amount = 0,
		.inst = NULL
};

static int _add(op_t op, int argc, var_t* a0, var_t* a1, var_t* a2);

int preliminary_gen(void)
{
	return vm_run();
}

static int _add(op_t op, int argc, var_t* a0, var_t* a1, var_t* a2)
{
	int strindex = pre_code.amount;
	if(pre_code.inst == NULL) {
		pre_code.inst = (inst_t*)calloc(1,sizeof(inst_t));
		pre_code.amount = 1;
	} else {
		pre_code.amount++;
		pre_code.inst =
				str_realloc(pre_code.inst, pre_code.amount * sizeof(inst_t));
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
	if(a2 != NULL && argc >= 3)
		pre_code.inst[strindex].arg2 = *a2;
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

int add_3(op_t op, var_t* var0, var_t* var1, var_t* var2)
{
	return _add(op, 3, var0, var1, var2);
}

int add_bin(op_t op, var_t* var0, var_t* var1)
{
	return _add(op, 2, var0, var1, NULL);
}

int add_un(op_t op, var_t*var0)
{
	return _add(op, 1, var0, NULL, NULL);
}

int add_noarg(op_t op)
{
	return _add(op, 0, NULL, NULL, NULL);
}

char* op_to_str(op_t op)
{
	switch(op) {
	case CMP_NEQ:
		return "CMPNEQ";
	case CMP_EQ:
		return "CMPEQ";
	case CMP_BEQ:
		return "CMPBEQ";
	case CMP_LEQ:
		return "CMPLEQ";
	case CMP_B:
		return "CMPB";
	case CMP_L:
		return "CMPL";
	case DIV:
		return "DIV";
	case REM:
		return "REM";
	case MUL:
		return "MUL";
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
	case EXPR_FINI:
		return "EXPR FINI";
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
		case 3:
			printf("<%s>%s%s", type_to_str(inst->arg0.type),
						var_get_inst(&inst->arg0),
						(inst->arg0.arrayel)?"[]":""
						);
			printf(",\t");
			printf("<%s>%s%s", type_to_str(inst->arg1.type),
						var_get_inst(&inst->arg1),
						(inst->arg1.arrayel)?"[]":""
						);
			printf(",\t");
			printf("<%s>%s%s", type_to_str(inst->arg2.type),
						var_get_inst(&inst->arg2),
						(inst->arg2.arrayel)?"[]":""
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
			regs_vm.regs[i].num = i;
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
	switch(regs_vm.regs[r].type) {
	case C_LONG_T:
		switch(r) {
		case R8:
			return "R8";
		case R9:
			return "R9";
		case R10:
			return "R10";
		case R11:
			return "R11";
		case R12:
			return "R12";
		case R13:
			return "R13";
		case R14:
			return "R14";
		case R15:
			return "R15";
		case RBP:
			return "RBP";
		case RSI:
			return "RSI";
		case RDI:
			return "RDI";
		case RAX:
			return "RAX";
		case RBX:
			return "RBX";
		case RCX:
			return "RCX";
		case RDX:
			return "RDX";
		}
		break;
	case C_INT_T:
	case C_SIGNED_T:
	case C_UNSIGNED_T:
		switch(r) {
		case R8:
			return "R8D";
		case R9:
			return "R9D";
		case R10:
			return "R10D";
		case R11:
			return "R11D";
		case R12:
			return "R12D";
		case R13:
			return "R13D";
		case R14:
			return "R14D";
		case R15:
			return "R15D";
		case RBP:
			return "EBP";
		case RSI:
			return "ESI";
		case RDI:
			return "EDI";
		case RAX:
			return "EAX";
		case RBX:
			return "EBX";
		case RCX:
			return "ECX";
		case RDX:
			return "EDX";
		}
		break;
	case C_SHORT_T:
		switch(r) {
		case R8:
			return "R8W";
		case R9:
			return "R9W";
		case R10:
			return "R10W";
		case R11:
			return "R11W";
		case R12:
			return "R12W";
		case R13:
			return "R13W";
		case R14:
			return "R14W";
		case R15:
			return "R15W";
		case RBP:
			return "BP";
		case RSI:
			return "SI";
		case RDI:
			return "DI";
		case RAX:
			return "AX";
		case RBX:
			return "BX";
		case RCX:
			return "CX";
		case RDX:
			return "DX";
		}
		break;
	case C_CHAR_T:
		switch(r) {
		case R8:
			return "R8B";
		case R9:
			return "R9B";
		case R10:
			return "R10B";
		case R11:
			return "R11B";
		case R12:
			return "R12B";
		case R13:
			return "R13B";
		case R14:
			return "R14B";
		case R15:
			return "R15B";
		case RBP:
			return "BPL";
		case RSI:
			return "SIL";
		case RDI:
			return "DIL";
		case RAX:
			return "AL";
		case RBX:
			return "BL";
		case RCX:
			return "CL";
		case RDX:
			return "DL";
		}
		break;
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
		if(str_get(num)->synt == S_CONST)
			var->memtype = CONSTANT;
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
	case CONSTANT:
		return str_get(var->num)->inst;
	default:
		return NULL;
	}
}

/***************************/

bool var_is_same(var_t* v0, var_t* v1)
{
	if((v0->memtype == v1->memtype) &&
			(v0->num == v1->num))
			return true;
	return false;
}
/* instructions */

int inc(var_t*to)
{
	return add_un(INC, to);
}

int dec(var_t*to)
{
	return add_un(DEC, to);
}

int sign(var_t*to, var_t*from)
{
	if(var_is_same(to, from))
		return add_un(SIGN, from);
	return add_bin(SIGN, to, from);
}

int mov(var_t*to, var_t*from)
{
	if(var_is_same(to, from))
		return to->num;
	return add_bin(MOV, to, from);
}

int conv(var_t*to, var_t*from)
{
	if(var_is_same(to, from) || from->type == from->conv)
		return to->num;
	return add_bin(CONV, to, from);
}

int binary_op(int opnum, var_t*to, var_t*from)
{
	op_t op = NOP;
	if(!strcmp(str_get(opnum)->inst, "+")) op = ADD;
	if(!strcmp(str_get(opnum)->inst, "-")) op = SUB;
	if(!strcmp(str_get(opnum)->inst, "/")) op = DIV;
	if(!strcmp(str_get(opnum)->inst, "*")) op = MUL;
	if(!strcmp(str_get(opnum)->inst, "%")) op = REM;
	if(!strcmp(str_get(opnum)->inst, "<<")) op = SHL;
	if(!strcmp(str_get(opnum)->inst, ">>")) op = SHR;
	return add_bin(op, to, from);
}

int comparison_op(int opnum, var_t*result, var_t*to, var_t*from)
{
	op_t op;
	if(!strcmp(str_get(opnum)->inst, "==")) op = CMP_EQ;
	if(!strcmp(str_get(opnum)->inst, "!=")) op = CMP_NEQ;
	if(!strcmp(str_get(opnum)->inst, "<=")) op = CMP_LEQ;
	if(!strcmp(str_get(opnum)->inst, ">=")) op = CMP_BEQ;
	if(!strcmp(str_get(opnum)->inst, "<")) op = CMP_L;
	if(!strcmp(str_get(opnum)->inst, ">")) op = CMP_B;
	return add_3(op, result, to, from);
}

var_t* get_arg(int num, int argnum)
{
	switch(argnum) {
	case 0:
		return &pre_code.inst[num].arg0;
	case 1:
		return &pre_code.inst[num].arg1;
	case 2:
		return &pre_code.inst[num].arg2;
	}
	return NULL;
}

op_t get_opcode(int num)
{
	return pre_code.inst[num].opcode;
}

inst_t* get_instr(int num)
{
	return &pre_code.inst[num];
}

char* var_to_str(var_t*v)
{
	switch(v->memtype) {
	case REGISTER:
		return reg_to_str(v->num);
	case MEMORY_LOC:
		return str_get(v->num)->inst;
	case CONSTANT:
		return str_get(v->num)->inst;
	}
}
