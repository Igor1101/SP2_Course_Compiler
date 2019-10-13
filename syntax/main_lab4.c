/*
 * main_lab2.c
 *
 *  Created on: Sep 24, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdlib.h>
#include <defs.h>
#include <stdbool.h>
#include <lexems/utf8.h>
#include <lexems/lex.h>
#include <lexems/tables.h>
#include <syntax/syntax.h>
#include <string.h>
#include "syntax.h"

void prg_exit(int r)
{
	exit(r);
}
int main(void)
{
	char code[1000];
	while(1) {
		printf("code:\t");
		memset(code, 0, sizeof code);
		if(fgets(code, sizeof code, stdin) == NULL) {
			prg_exit(0);
		}
        int lerr_amount = lex_parse(code);
		pr_info("lexical error amount: %d", lerr_amount);
		/* print result */
		for(int i=0; i<str_array.amount;i++) {
			pr_info("lexem:\t%s\t\t:\t<%s> ", str_get_inst(i), lex_to_str(str_get(i)->lext));
		}
        if(lerr_amount > 0) {
            pr_err("lerrors detected, wont start syntax analyzer");
            return lerr_amount;
        }
	}
	return 0;
};


