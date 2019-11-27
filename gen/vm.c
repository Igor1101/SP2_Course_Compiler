/*
 * vm.c
 *
 *  Created on: Nov 23, 2019
 *      Author: igor
 */
#include <stdbool.h>
#include <defs.h>
#include <lexems/tables.h>
#include <semantic/tables_sem.h>
#include <string.h>
#include "prelim.h"

static int next_var_expr(int num);
static int prev_var_expr(int num);
static int process_expression(int num, bool param);
static int process_declaration(int num, bool param);
static int next_binop(int start, int end);
static int last_assignment(int start, int end);

int vm_run(void)
{
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_KEYWORD_TYPE:
			num = process_declaration(num, false);
			break;
		case S_OPERAT_ASSIGNMENT:
		{
			num = process_expression(num-1, false);
			add_noarg(EXPR_FINI);
			break;
		}
		case S_ID_ARRAY:
		case S_ID_FUNCTION:
		case S_ID_PARAM:
		case S_ID_VARIABLE:
		case S_OPERAT_BINARY:
		case S_OPERAT_UNARY:
		{
			pr_debug("expr detected");
			num = process_expression(num, false);
			add_noarg(EXPR_FINI);
			break;
		}
		default:
			num++;
		}
	}
	return 0;
}

struct var_node{
	int varnum;
	bool in_reg;
	int reg;
	struct var_node* next;
};

static int process_expression(int num, bool param)
{
	int savenum = num;
	bool has_assignment=false;
	var_t rvalue;
	var_t lvalue;
	ctypes_t main_type;
	struct var_node* var0 = calloc(1, sizeof(struct var_node));
	void free_vars(void)
	{
		struct var_node*i = var0;
		while(i->next != NULL) {
			struct var_node*pr = i;
			i = i->next;
			free_reg(pr->reg);
			str_free((void**)&pr);
		}
		free(i);
	}
	int set_var_reg(int reg, int varnum)
	{
		struct var_node*i = var0;
		do {
			if(i->varnum == varnum && !i->in_reg) {
				i->reg = reg;
				i->in_reg = true;
				return 0;
			}
			i = i->next;
		} while(i != NULL);
		return -1;
	}
	void get_vars(void)
	{
		struct var_node* prev = var0;
		for(num=savenum; num<next_delimiter(num, 0, param);num++) {
			if(str_get(num)->synt == S_ID_VARIABLE) {
				prev->varnum = num;
				prev->next = calloc(1, sizeof (struct var_node));
				prev = prev->next;
				prev->next = NULL;
				prev->varnum = -1;
				prev->reg = -1;
			}
		}
	}
	bool var_has_reg(int varnum)
	{
		struct var_node*i = var0;
		do {
			if(i->varnum == varnum && !i->in_reg) {
				return false;
			}
			if(i->varnum == varnum && i->in_reg) {
				return true;
			}
			i = i->next;
		} while(i != NULL);
		return false;
	}
	int var_get_local(int num, int mem, var_t*var)
	{
		if(mem == MEMORY_LOC) {
			if(var_has_reg(num)) {
				struct var_node*i = var0;
				do {
					if(i->varnum == num && i->in_reg) {
						pr_debug("using reg instead of var");
						num = i->reg;
						mem = REGISTER;
					}
					i = i->next;
				} while(i != NULL);
			}
		}
		return var_get(num, mem, var);
	}
	/* get all vars */
	get_vars();
	/* unary op first ++<var> (changeable) */
	for(num=savenum;num<next_delimiter(num, 0, param);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
				if(!strcmp(str_get(num)->inst, "++")) {
					var_t v;
					var_get_local(var, MEMORY_LOC, &v);
					inc(&v);
				}
				if(!strcmp(str_get(num)->inst, "--")) {
					var_t v;
					var_get_local(var, MEMORY_LOC, &v);
					dec(&v);
				}
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	/* assignment info get */
	for(num=savenum;num<next_delimiter(num, 0, false);num++) {
		if(str_get(num)->synt == S_OPERAT_ASSIGNMENT) {
			has_assignment = true;
			/* get lvalue */
			if(!strcmp(str_get(num-1)->inst, "]")) {
				/*TODO this is array */
			}
			main_type = str_get(num-1)->ctype;
			break;
		}
	}

	/* unary op  (unchangeable) */
	for(num=savenum;num<next_delimiter(num, 0, false);) {
		switch(str_get(num)->synt) {
		case S_OPERAT_UNARY:
		{
			int var = num+1;
			if(str_get(var)->lext == L_IDENTIFIER) {
					/*ignore + */
				if(!strcmp(str_get(num)->inst, "-")) {
					int reg = reserve_reg(str_get(var)->ctype);
					var_t from, to;
					var_get_local(var, MEMORY_LOC, &from);
					var_get_local(reg, REGISTER, &to);
					sign(&to, &from);
					set_var_reg(reg, var);
				}
			}
			num++;
			break;
		}
		default:
			num++;
		}
	}
	/* conversion op */
	for(num=savenum;num<next_delimiter(num, 0, false);num++) {
		if(str_get(num)->conv_to != C_UKNOWN &&
				str_get(num)->lext == L_IDENTIFIER) {
			int cvt = num;
			var_t from, to;
			int reg_to = reserve_reg(str_get(num)->conv_to);
			var_get_local(reg_to, REGISTER, &to);
			var_get_local(cvt, MEMORY_LOC, &from);
			conv(&to, &from);
			set_var_reg(reg_to, cvt);
		}
	}

	int min_binop_level = 5000;
	struct region {
		int start;
		int end;
		int level;
		int reg_data;
		struct region*next;
	}region0;
	region0.next = NULL;
	void add_region(int start, int end, int level, int data)
	{
		struct region* r = &region0;
		do {
			if(r->next == NULL) {
				r->next = calloc(1, sizeof(struct region));
				break;
			}
		} while(r!=NULL);
	}
	/* get min level of binary op */
	for(num=savenum;num<next_delimiter(num, 0, false);num++) {
		if(str_get(num)->synt == S_OPERAT_BINARY) {
			if(str_get(num)->level < min_binop_level)
				min_binop_level = str_get(num)->level;
		}
	}
	/* bin op run */

	int get_rvalue(int start, int end, int level, var_t* result)
	{
		bool firstop = true;
		if(next_binop(start, end) < 0) {
			/* looks like no operation, just number */
			for(int num=start; num<end; num++) {
				if(str_get(num)->lext == L_IDENTIFIER ||
						str_get(num)->synt == S_CONST) {
					var_t from;
					var_get_local(num, MEMORY_LOC, &from);
					mov(result, &from);
					return ++num;
				}
			}
		}
		for(int num=start; num<end; num++) {
			int num_after;
			if(next_binop(num, end) < 0) {
				return num;
			}
			/*
			int nxtlevel = str_get(next_binop(num, end))->level;
			if(nxtlevel > level) {
				var_t local_res;
				num = get_rvalue(num, end, nxtlevel, &local_res);
			}*/
			if(str_get(num)->synt == S_OPERAT_BINARY && str_get(num)->lext != L_OPERAT_RELATION) {
				int op_num = num;
				/* use current function to calc*/
				if(str_get(num)->level >= level) {
					int var_prev = prev_var_expr(num);
					int var_nxt = next_var_expr(num);
					var_t from, to;
					var_get_local(var_prev, MEMORY_LOC, &to);
					int nxtlevel = str_get(next_binop(num+1, end))->level;
					if(next_binop(num+1, end) >=0 && nxtlevel > level) {
						int reg = reserve_reg(main_type);
						var_get_local(reg, REGISTER, &from);
						num = get_rvalue(num+1, end, nxtlevel, &from);
					} else
						var_get_local(var_nxt, MEMORY_LOC, &from);
					if(firstop)
						mov(result, &to);
					firstop = false;
					binary_op(op_num, result, &from);
				} else {
					return num-1;
				}
			} else if(str_get(num)->lext == L_OPERAT_RELATION) {
				int op_num = num;
					/* use current function to calc*/
				if(str_get(num)->level >= level) {
					int var_prev = prev_var_expr(num);
					int var_nxt = next_var_expr(num);
					var_t comp1, comp2;
					var_get_local(var_prev, MEMORY_LOC, &comp1);
					int nxtlevel = str_get(next_binop(num+1, end))->level;
					if(next_binop(num+1, end) >=0 && nxtlevel > level) {
						int reg = reserve_reg(main_type);
						var_get_local(reg, REGISTER, &comp2);
						num = get_rvalue(num+1, end, nxtlevel, &comp2);
					} else
						var_get_local(var_nxt, MEMORY_LOC, &comp2);
					firstop = false;
					comparison_op(op_num, result, &comp1, &comp2);
				} else {
					return num-1;
				}
			}
		}
		return num;
	}
	int reg_result = reserve_reg(main_type);
	var_get_local(reg_result, REGISTER, &rvalue);
	int startfrom = 1 + last_assignment(savenum, next_delimiter(num, 0, false));
	get_rvalue(startfrom, next_delimiter(num, 0, false), min_binop_level,
			&rvalue);
	/* assignment op */
	for(num=savenum;num<next_delimiter(num, 0, false);num++) {
		if(str_get(num)->synt == S_OPERAT_ASSIGNMENT) {
			int result = num - 1;
			var_get(result, MEMORY_LOC, &lvalue);
			mov(&lvalue, &rvalue);
		}
	}
	free_vars();
	return num;
}

static int next_binop(int start, int end)
{
	for(int num=start; num<end; num++)  {
		if(str_get(num)->synt == S_OPERAT_BINARY ||
				str_get(num)->synt == S_OPERAT_RELATION)
			return num;
	}
	return -1;
}

static int last_assignment(int start, int end)
{
	for(int num=end; num>start; --num) {
		if(str_get(num)->synt == S_OPERAT_ASSIGNMENT) {
			return num;
		}
	}
	return -1;
}

static int prev_var_expr(int num)
{
	do {
		num--;
		if(str_get(num)->synt == S_ID_VARIABLE
				|| str_get(num)->synt == S_CONST)
			return num;
	} while(true);
}

static int next_var_expr(int num)
{
	do {
		num++;
		if(str_get(num)->synt == S_ID_VARIABLE
				|| str_get(num)->synt == S_CONST)
			return num;
	} while(true);
}

static int process_declaration(int num, bool param)
{
	return ++num;
}
