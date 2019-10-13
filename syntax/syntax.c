/*
 * analyzer.c
 *
 *  Created on: Oct 9, 2019
 *      Author: igor
 */
#include <lexems/lex.h>
#include <lexems/tables.h>

int analyze(void)
{
	int err_amount = 0;
	for(int i=0; i<str_array.amount; i++) {
		switch(str_get(i)->lext) {
		case L_BRACE_CLOSING:
			close_brace(str_get(i)->inst);
		case L_BRACE_OPENING:
			open_brace(str_get(i)->inst);

		}
    }
	return err_amount;
}
