/*
 * main_lab2.c
 *
 *  Created on: Sep 24, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdlib.h>
#include <trees/MooreMachine.h>
#include <lexems/cli.h>
#include <defs.h>

void prg_exit(int r)
{
	exit(r);
}
int main(void)
{
	while(1) {
		printf("-> ");
		if(fgets(cli_line, sizeof cli_line, stdin) == NULL) {
			prg_exit(0);
		}
		cli_get_words();
		if(CLI_IS("sig")) {
			MOORE_STATE lastst = moore_getstate();
			moore_next_state_move_str(ARG_1);
			pr_info(" sig %s State %s => State: %s",
					ARG_1,
					moore_state_to_str(lastst),
					moore_state_to_str(moore_getstate()));
		}
		if(CLI_IS("sigall")) {
			for(int i=1; i< CLI_WORDS;i++) {
				if(strlen(cli_word[i])==0) {
					pr_info("no more signals");
					break;
				}
				MOORE_STATE lastst = moore_getstate();
				moore_next_state_move_str(cli_word[i]);
				pr_info(" sig %s State %s => State: %s",
					cli_word[i],
					moore_state_to_str(lastst),
					moore_state_to_str(moore_getstate()));
			}
		}
	}
	return 0;
};

