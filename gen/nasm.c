/*
 * nasm.c
 *
 *  Created on: Nov 27, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <limits.h>
#include "prelim.h"
#include "nasm.h"
#include <lexems/tables.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <semantic/tables_sem.h>

r_stat_t rstate[ALL_REGS_AMOUNT];
FILE* asmfile;

static int process_cmd(int num);
static void process_mov(int num);
static void process_exprfini(int num);
static void process_add(int num);
static void process_mul(int num);
static void process_sub(int num);
static void process_eq(int num);
static void process_dec(int num);
static void process_inc(int num);

static void mov_int(var_t * a0, var_t* a1);
static void mov_floating(var_t * a0, var_t* a1);
static void cmp(var_t* a0, var_t* a1);
static void sete(var_t* v);

static char* var_to_str_offset(var_t*v);

int gen_nasm(void)
{
	asmfile = fopen("result.asm", "w");
	out_decl();
	out_init();
	for(int i=0; i<pre_code.amount; ) {
		set_regs_used(i);
		i = process_cmd(i);
	}
	out_decl_printf();
	out_deinit();
	fclose(asmfile);
	return 0;
}

void set_regs_used(int num)
{
	/* used registers must be preserved */
	inst_t* i = get_instr(num);
	if(i->argc >= 1 && i->arg0.memtype == REGISTER) {
		rstate[i->arg0.num].used = true;
	}
	if(i->argc >= 2 && i->arg1.memtype == REGISTER) {
		rstate[i->arg1.num].used = true;
	}
	if(i->argc >= 3 && i->arg2.memtype == REGISTER) {
		rstate[i->arg2.num].used = true;
	}
}

static int process_cmd(int num)
{
	switch(get_opcode(num)) {
	case NOP:
		outop0("NOP");
		break;
	case MOV:
		pr_debug("processing mov");
		process_mov(num);
		break;
	case EXPR_FINI:
		pr_debug("processing fini");
		process_exprfini(num);
		break;
	case ADD:
		pr_debug("processing add");
		process_add(num);
		break;
	case SUB:
		pr_debug("processing sub");
		process_sub(num);
		break;
	case MUL:
		pr_debug("processing mul");
		process_mul(num);
		break;
	case CMP_EQ:
		process_eq(num);
		break;
	case DEC:
		pr_debug("processing dec");
		process_dec(num);
		break;
	case INC:
		pr_debug("processing dec");
		process_inc(num);
		break;
	}
	return ++num;
}

// A structure to represent a stack
struct Stack {
    r64_t top;
    unsigned capacity;
    r64_t* array;
};
static struct Stack* stack ;
// function to create a stack of given capacity. It initializes size of
// stack as 0
void stackalloc(unsigned capacity)
{
    stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (r64_t*)malloc(stack->capacity * sizeof(r64_t));
}
void stackfree(void)
{
	str_free((void**)&stack->array);
	str_free((void**)&stack);
}

// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
void push(r64_t reg)
{
    if (isFull(stack)) {
    	pr_err("stack is full!");
        return;
    }
    stack->array[++stack->top] = reg;
    pr_debug("%d pushed to stack", reg);
    /* now push it really to stack:) */
    out("PUSH\t%s\n", reg_to_str(reg));
}

// Function to remove an item from stack.  It decreases top by 1
r64_t pop(void)
{
    if (isEmpty(stack))
        return INT_MIN;
    r64_t reg = stack->array[stack->top--];
    out("POP\t%s\n", reg_to_str(reg));
    return reg;
}

// Function to return the top from stack without removing it
r64_t peek(void)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}

int regsafetely_use(r64_t reg)
{
	if(rstate[reg].used) {
		push(reg);
		rstate[reg].pushedtimes++;
	}
	return rstate[reg].pushedtimes;
}

r64_t regsafetely_unuse(r64_t reg)
{
	if(rstate[reg].used) {
		r64_t regpoped = pop();
		assert(reg == regpoped);
		return regpoped;
	}
	return reg;
}

static void process_mov(int num)
{
	assert(get_instr(num)->argc == 2);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	ctypes_t t = a0->type;
	switch (t) {
		case C_DOUBLE_T:
		case C_FLOAT_T:
			mov_floating(a0, a1);
			break;
		default:
			mov_int(a0, a1);
			break;
	}
}

void out_decl(void)
{
	out("section .data\n");
	for(int i=0; i<ident_array.amount; i++) {
		ident_t* id = ident_get(i);
		out("%s: TIMES %d ", id->inst, id->amount);
		switch(id->type) {
		case C_CHAR_T:
			out("DB");
			break;
		case C_INT_T:
		case C_UNSIGNED_T:
		case C_SIGNED_T:
			out("DD");
			break;
		case C_DOUBLE_T:
		case C_FLOAT_T:
		case C_LONG_T:
			out("DQ");
			break;
		case C_SHORT_T:
			out("DW");
			break;
		case C_UKNOWN:
			out("DQ");
			break;
		}
		out(" 0 \n");
	}
	/*out("\nsection .text\n");
	for(int i=0; i<ident_array.amount; i++) {
		out("extern %s\n", ident_get(i)->inst);
	}*/
}

void out_init(void)
{
	out("section .text\n");
	out("extern exit\n");
	out("extern printf\n");
	out("global main\n");
	out("main:\n");
	out("\n");
	out("\n");
}

void out_deinit(void)
{
	out("\n");
	out("\n");
	out("MOV RCX, 0\n");
	out("CALL exit\n");
	out("RET\n");
}

void out_decl_printf(void)
{
#define STR_DATA "STR_%s_%d"
	/* setting rodata strings for vars */
	for(int j=0; j<ident_array.amount; j++) {
		ident_t* id = ident_get(j);
		for(int i=0; i<id->amount; i++){
			out("section .rodata\n");
			if(id->array)
				out(STR_DATA ": DB \"%s[%i]=%%", id->inst, i, id->inst, i);
			else
				out(STR_DATA ": DB \"%s=%%", id->inst, i, id->inst);
			switch(id->type) {
			case C_CHAR_T:
				out("c");
				break;
			case C_INT_T:
			case C_UNSIGNED_T:
			case C_SIGNED_T:
				out("d");
				break;
			case C_LONG_T:
				out("ld");
				break;
			case C_SHORT_T:
				out("hi");
				break;
			case C_UKNOWN:
				out("d");
				break;
			case C_DOUBLE_T:
			case C_FLOAT_T:
				out("f");
				break;
			}
			out("\", 0xA, 0\n");

			/* setting output of them */
			out("section .text\n");
			ident_t* id = ident_get(j);
			switch(id->type) {
			case C_DOUBLE_T:
			case C_FLOAT_T:
				out("MOV\tRAX,\t1\n");
				out("MOV\tRBX,\t[%s+%s%s%d]\n", id->inst,
						id->array?ctype_sz(id->type):"",
						id->array?"*":"",
						i);
				out("MOVQ\tXMM0,\tRBX\n");
				break;
			default:
				out("MOV\tRAX,\t0\n");
				out("MOV\tRSI,\t[%s+%s%s%d]\n", id->inst,
						id->array?ctype_sz(id->type):"",
						id->array?"*":"",
						i);
			}
			out("MOV\tRDI,\t" STR_DATA "\n", id->inst, i);
			out("CALL printf\n");
		}
	}
}

static void process_exprfini(int num)
{
	for(int i=0; i<REGS_AMOUNT; i++) {
		rstate[i].used = false;
	}
	out("; END OF EXPRESSION\n\n");
}

static void process_add(int num)
{
	assert(get_instr(num)->argc == 2);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	out("ADD    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
}

static void process_mul(int num)
{
	assert(get_instr(num)->argc == 2);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	var_t rax;
	get_reg_force(RAX, a1->type);
	var_get(RAX, REGISTER, &rax);
	regsafetely_use(RAX);
	regsafetely_use(RDX);
	out("MOV   %s,    %s\n", var_to_str_offset(&rax), var_to_str_offset(a1));
	out("MUL    %s\n", var_to_str_offset(a0));
	out("MOV   %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(&rax));
	regsafetely_unuse(RDX);
	regsafetely_unuse(RAX);
}

static void process_sub(int num)
{
	assert(get_instr(num)->argc == 2);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	out("SUB    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
}

static void mov_int(var_t * a0, var_t* a1)
{
	out("MOV    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
}

static void mov_floating(var_t * a0, var_t* a1)
{
	static int floating_num = 0;
	if(a0->memtype == MEMORY_LOC
		&& a1->memtype == REGISTER) {
		out("MOVQ   %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
	} else if(a0->memtype == REGISTER
		&& a1->memtype == MEMORY_LOC) {
		out("MOVQ    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
	} else if(a0->memtype == REGISTER
		&& a1->memtype == CONSTANT) {
		out("section .rodata\n");
		out("FLOATING_NUM_%d: DQ %s\n", floating_num, var_to_str_offset(a1));
		out("section .text\n");
		regsafetely_use(R8);
		var_t* r8 = get_reg_force(R8, C_LONG_T);
		out("MOV    %s,    [FLOATING_NUM_%d]\n", var_to_str_offset(r8), floating_num);
		out("MOVQ    %s,   %s\n", var_to_str_offset(a0), var_to_str_offset(r8));
		regsafetely_unuse(R8);
	} else if(a0->memtype == REGISTER
		&& a1->memtype == REGISTER) {
		out("MOVQ    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
	}
	floating_num++;
}

char* ctype_sz(ctypes_t t)
{
	switch(t) {
	case C_UNSIGNED_T:
		return "4";
	case C_UKNOWN:
		return "8";
	case C_SIGNED_T:
		return "4";
	case C_SHORT_T:
		return "2";
	case C_LONG_T:
		return "8";
	case C_INT_T:
		return "4";
	case C_FLOAT_T:
		return "8";
	case C_DOUBLE_T:
		return "8";
	case C_CHAR_T:
		return "1";
	}
	return 0;
}

static void process_eq(int num)
{
	assert(get_instr(num)->argc == 3);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	var_t* a2 = get_arg(num, 2);
	cmp(a1, a2);
	sete(a0);
}

static void sete(var_t* v)
{
	ctypes_t maintype = v->type;
	int regnum = v->num;
	if(v->memtype == REGISTER) {
		/* change type temporarily */
		v = get_reg_force(v->num, C_CHAR_T);
		out("SETE    %s\n", var_to_str_offset(v));
	} else {
		if(regnum!= RAX)
			regsafetely_use(RAX);
		var_t* rax = get_reg_force(RAX, C_CHAR_T);
		out("SETE    %s\n", var_to_str_offset(rax));
		mov_int(v, rax);
		if(regnum!= RAX)
			regsafetely_unuse(RAX);
	}
	/* unchange type */
	v = get_reg_force(v->num, maintype);
}

static void cmp(var_t* a0, var_t* a1)
{
	out("CMP    %s,    %s\n", var_to_str_offset(a0), var_to_str_offset(a1));
}

static void process_dec(int num)
{
	assert(get_instr(num)->argc == 1);
	var_t* arg = get_arg(num, 0);
	out("DEC    %s    %s\n", type_to_nasmtype(arg->type), var_to_str_offset(arg));
}

static void process_inc(int num)
{
	assert(get_instr(num)->argc == 1);
	var_t* arg = get_arg(num, 0);
	out("INC    %s    %s\n", type_to_nasmtype(arg->type), var_to_str_offset(arg));
}
/*
 * var_to_str_offset decorator
 * array offset calculation if needed
 */
static char* var_to_str_offset(var_t*v)
{
#define BUFS 4
	static int buf_current = 0;
	static char outbuf[BUFS][100];
	if(buf_current >= BUFS)
		buf_current = 0;
	memset(outbuf[buf_current], 0 , sizeof outbuf[buf_current]);
	snprintf(outbuf[buf_current], sizeof outbuf[buf_current], "%s%s%s%s%s%s%s",
			(v->memtype==MEMORY_LOC)?"[":"",
			var_to_str(v),
			v->arrayel?"+":"",
			v->arrayel?reg_to_str(v->arrreg_offset):"",
			v->arrayel?"*":"",
			v->arrayel?ctype_sz(v->type):"",
			(v->memtype==MEMORY_LOC)?"]":""
			);
	return outbuf[buf_current++];
#undef BUFS
}

char* type_to_nasmtype(ctypes_t t)
{
	switch(t) {
	case C_CHAR_T:
		return "BYTE";
	case C_DOUBLE_T:
		return "QWORD";
	case C_FLOAT_T:
		return "DWORD";
	case C_INT_T:
		return "DWORD";
	case C_LONG_T:
		return "QWORD";
	case C_SHORT_T:
		return "WORD";
	case C_SIGNED_T:
		return "DWORD";
	case C_UKNOWN:
		return "QWORD";
	case C_UNSIGNED_T:
		return "DWORD";
	}
}
