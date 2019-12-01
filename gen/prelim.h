/*
 * prelim.h
 *
 *  Created on: Nov 16, 2019
 *      Author: igor
 */

#ifndef GEN_PRELIM_H_
#define GEN_PRELIM_H_

#include <defs.h>
#include <semantic/tables_sem.h>

#define REGS_AMOUNT 15
#define R8 0
#define R9 1
#define R10 2
#define R11 3
#define R12 4
#define R13 5
#define R14 6
#define R15 7
#define RBP 8
#define RSI 9
#define RDI 10
#define RAX 11
#define RBX 12
#define RCX 13
#define RDX 14

typedef enum {
	NOP,
	EXPR_FINI,
	CMP_EQ,
	CMP_NEQ,
	CMP_B,
	CMP_L,
	CMP_BEQ,
	CMP_LEQ,
	DIV,
	REM,
	MUL,
	INC,
	DEC,
	CONV,
	MOV,
	PUSH,
	POP,
	ADD,
	SUB,
	ADDS,
	SUBS,
	SHR,
	SHL,
	AND,
	OR,
	XOR,
	NOR,
	NOT,
	SIGN,
	JMP,
	CALL,
	RET,
	JZ
}op_t;
typedef enum {
	REGISTER,
	MEMORY_LOC,
	STACK_INST,
	CONSTANT
}mem_t;

typedef struct {
	int num;
	ctypes_t type;
	ctypes_t conv;
	mem_t memtype;
	bool pointer;
	bool arrayel;
} var_t;

typedef struct {
	op_t opcode;
	var_t arg0;
	var_t arg1;
	var_t arg2;
	int argc;
}inst_t;

typedef struct {
	inst_t* inst;
	int amount;
}code_t;


extern code_t pre_code;

/* utils */
ctypes_t str_get_memtype(int num);
char* var_get_inst(var_t *var);
int var_get(int num, mem_t mem, var_t* var);
int add_bin(op_t op, var_t* var0, var_t* var1);
int add_un(op_t op, var_t*var0);
int add_noarg(op_t op);
char* op_to_str(op_t op);
void prelim_print_debug(void);
int reserve_reg(ctypes_t type);
void free_reg(int r);
var_t* get_reg(int num);
var_t* get_reg_force(int num, ctypes_t type);
char* reg_to_str(int r);
char* var_to_str(var_t*v);
void free_ops(void);
void init_prelim(void);
var_t* get_arg(int num, int argnum);
op_t get_opcode(int num);
inst_t* get_instr(int num);
/* lan specific */
int preliminary_gen(void);

/* instr */

int inc(var_t*to);
int dec(var_t*to);
int sign(var_t*to, var_t*from);
int mov(var_t*to, var_t*from);
int conv(var_t*to, var_t*from);
int binary_op(int opnum, var_t*to, var_t*from);
int comparison_op(int opnum, var_t*result, var_t*to, var_t*from);

#endif /* GEN_PRELIM_H_ */
