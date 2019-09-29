/*
 * main_lab2.c
 *
 *  Created on: Sep 24, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdlib.h>
#include <lexems/MooreMachine.h>
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
		moore_reset();
		int i=0;
		while(true) {
			unsigned ch = u8_nextchar(code, &i);
			if(ch == 0)
				break;
			MOORE_SIGNAL sig = ch_to_moore_signal(ch);
			MOORE_STATE oldst = moore_getstate();
			MOORE_STATE st = moore_next_state_move(sig);
			printf("sig=%s from %s to %s\n",
					moore_sig_to_str(sig),
					moore_state_to_str(oldst),
					moore_state_to_str(st));
		}
	}
	return 0;
};


