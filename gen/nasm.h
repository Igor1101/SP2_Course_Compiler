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
	int pushedtimes;
}r_stat_t;

extern r_stat_t rstate[ALL_REGS_AMOUNT];
extern FILE* asmfile;


void set_regs_used(int num);

int gen_nasm(void);
void out_decl(void);
void out_init(void);
void out_deinit(void);
void out_decl_printf(void);

#endif /* GEN_NASM_H_ */
