/*
 * nasm.h
 *
 *  Created on: Nov 27, 2019
 *      Author: igor
 */

#ifndef GEN_NASM_H_
#define GEN_NASM_H_

#include <defs.h>
#include <stdio.h>
#include "prelim.h"

#define out(...) fprintf(asmfile, __VA_ARGS__)
#define outop0(op) out("%s\n", op)
#define outop1(op, a) out("%s    %s\n", op, a)
#define outop2(op, a0, a1) out("%s    %s,   %s\n", op, a0, a1)
typedef int r64_t;

typedef struct {
	bool used;
	bool pushed;
}r_stat_t;

extern r_stat_t rstate[REGS_AMOUNT];
extern FILE* asmfile;


int gen_nasm(void);

#endif /* GEN_NASM_H_ */
