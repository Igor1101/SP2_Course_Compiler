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
	inst_t inst[];
	int amount;
}code_t;
extern code_t pre_code;


#endif /* GEN_PRELIM_H_ */
