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

void prg_exit(int r)
{
	str_array_remove();
	exit(r);
}
int main(void)
{
	char code[1000];
	while(1) {
		printf("code:\t");
		memset(code, 0, sizeof code);
		//memcpy(code, "a++;b++;",strlen("f(sd,ds)"));
		if(fgets(code, sizeof code, stdin) == NULL) {
			prg_exit(0);
		}
        int lerr_amount = lex_parse(code);
		pr_debug("lexical error amount: %d", lerr_amount);
		/* print result */
		for(int i=0; i<str_array.amount;i++) {
			pr_debug("lexem:\t%s\t\t:\t<%s> ", str_get_inst(i), lex_to_str(str_get(i)->lext));
		}
        if(lerr_amount > 0) {
        	lex_printf();
            pr_err("lerrors detected, wont start syntax analyzer");
            prg_exit(lerr_amount);
        }
        int serr_amount = syn_analyze();
		pr_info("syntax error amount: %d", serr_amount);
		for(int i=0; i<str_array.amount;i++) {
#if (defined DEBUG_APP)
			for(int j=0; j<str_get(i)->level; j++) {
				printf(" ");// print level
			}
			pr_info("%s\t:\t<%s>\t<%s> ", str_get_inst(i),
					lex_to_str(str_get(i)->lext), syn_to_str(str_get(i)->synt));
#endif
		}
		if(serr_amount>0) {
			syn_printf();
			pr_err("Syntax errors detected, wont start semantic analyzer");
			prg_exit(serr_amount);
		}
	}
	return 0;
};


