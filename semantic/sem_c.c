/*
 * sem_c.c
 *
 *  Created on: Nov 5, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <defs.h>
#include <lexems/tables.h>
#include "tables_sem.h"

static int process_declaration(int num);

int sem_analyze(void)
{
	ident_array_remove();
	for(int num=0; num<str_array.amount;) {
		switch(str_get(num)->synt) {
		case S_KEYWORD_TYPE:
			num = process_declaration(num);
		}
	}
}

static int process_declaration(int num)
{
	/*
	for(; num<next_delimiter(num, 0, false); num++) {
		if(str_get(num)->synt == S_ID_VARIABLE) {
			ident_add(str_get(num)->inst, );
		}
	}*/
}
