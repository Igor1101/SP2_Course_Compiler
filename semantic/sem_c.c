/*
 * sem_c.c
 *
 *  Created on: Nov 5, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdbool.h>
#include <defs.h>
#include <lexems/tables.h>
#include "sem.h"
#include "tables_sem.h"

static int process_declaration(int num);
static int process_expression(int num);

int sem_analyze(void)
{
	ident_array_remove();
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_KEYWORD_TYPE:
			num = process_declaration(num);
		default:
			num++;
		}
	}
}

static int process_declaration(int num)
{
	/* getting type of variables */
	ctypes_t t = str_to_type(str_get(num)->inst);
	/* getting declared vars */
	for(; num<next_delimiter(num, 0, false); num++) {
		switch(str_get(num)->synt) {
		case S_ID_VARIABLE:
			if(str_get(num+1)->synt == S_OPERAT_ASSIGNMENT) {
				if(ident_add(str_get_inst(num), t, true)<0) {
					pr_err("variable already initialized");
				}
				num = process_expression(num);
			} else {
				if(ident_add(str_get_inst(num), t, false)<0) {
					pr_err("variable already initialized");
				}
			}
		}
	}
}

static int process_expression(int num)
{
	num++;
	//switch(str_get(num)->)
}
