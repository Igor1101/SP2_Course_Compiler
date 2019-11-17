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

typedef enum {
	NOP,
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
	CONSTANT
}_argt_t;

typedef struct {
	_argt_t type;
	ctypes_t ctype;
	char*inst;
}arg_t;
typedef struct {
	op_t opcode;
	arg_t arg0;
	arg_t arg1;
	int argc;
}inst_t;

typedef struct {
	inst_t* inst;
	int amount;
}code_t;
extern code_t pre_code;

/* utils */
int add_bin(op_t op, int num0, char *num0_reg, int num1, char* num1_reg);
int add_un(op_t op, int num0, char *num0_reg);
int add_noarg(op_t op);
char* op_to_str(op_t op);
void prelim_print_debug(void);
/* lan specific */
int preliminary_gen(void);

#endif /* GEN_PRELIM_H_ */
