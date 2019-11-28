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

r_stat_t rstate[REGS_AMOUNT];
FILE* asmfile;

static int process_cmd(int num);
static void process_mov(int num);

int gen_nasm(void)
{
	asmfile = stdout;
	for(int i=0; i<pre_code.amount; ) {
		set_regs_used(i);
		i = process_cmd(i);
	}
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
		process_mov(num);

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
void push(struct Stack* stack, r64_t reg)
{
    if (isFull(stack)) {
    	pr_err("stack is full!");
        return;
    }
    stack->array[++stack->top] = reg;
    pr_debug("%d pushed to stack", reg);
}

// Function to remove an item from stack.  It decreases top by 1
r64_t pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

// Function to return the top from stack without removing it
r64_t peek(struct Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}

static void process_mov(int num)
{
	assert(get_instr(num)->argc == 2);
	var_t* a0 = get_arg(num, 0);
	var_t* a1 = get_arg(num, 1);
	if(a0->memtype == MEMORY_LOC
		&& a1->memtype == REGISTER) {
		out("MOV   [%s],    %s\n", var_to_str(a0), var_to_str(a1));
	} else if(a0->memtype == REGISTER
		&& a1->memtype == MEMORY_LOC) {
		out("MOV    %s,    [%s]\n", var_to_str(a0), var_to_str(a1));
	}
}

