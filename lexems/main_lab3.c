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

void prg_exit(int r)
{
	exit(r);
}
int main(void)
{
	char code[1000];
	while(1) {
		printf("code:\t");
		if(fgets(code, sizeof code, stdin) == NULL) {
			prg_exit(0);
		}
		lex_parse(code);
	}
	return 0;
};


