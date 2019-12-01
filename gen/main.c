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
#include <semantic/sem.h>
#include <string.h>
#include <gen/prelim.h>
#include <gen/nasm.h>

void prg_exit(int r)
{
	str_array_remove();
	ident_array_remove();
	free_ops();
	exit(r);
}
int main(int argc, char**argv)
{
	/* verify args */
	if(argc != 2) {
		pr_err("invalid args, use sp2cc <file>.c");
		return -1;
	}
	/* declare a file pointer */
	FILE    *infile;
	char    *buffer;
	long    numbytes;

	/* open an existing file for reading */
	infile = fopen(argv[1], "r");

	/* quit if the file does not exist */
	if(infile == NULL){
		pr_err("err opening file %s", argv[1]);
	    return -1;
	}

	/* Get the number of bytes */
	fseek(infile, 0L, SEEK_END);
	numbytes = ftell(infile);

	/* reset the file position indicator to
	the beginning of the file */
	fseek(infile, 0L, SEEK_SET);

	/* grab sufficient memory for the
	buffer to hold the text */
	buffer = (char*)calloc(numbytes+1, 1);

	/* memory error */
	if(buffer == NULL) {
		pr_err("cant calloc buffer sz=%ld", numbytes);
	}

	/* copy all the text into the buffer */
	fread(buffer, 1, numbytes, infile);
	fclose(infile);

    int lerr_amount = lex_parse(buffer);
	pr_debug("lexical error amount: %d", lerr_amount);
	/* free the memory we used for the buffer */
	free(buffer);
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
	if(serr_amount>0) {
		syn_printf();
		pr_err("Syntax errors detected, wont start semantic analyzer");
		prg_exit(serr_amount);
	}
	int sem_err_amount = sem_analyze();
	if(sem_err_amount > 0) {
		sem_printf();
		pr_err("Semantic errors detected, wont start code gen");
		prg_exit(sem_err_amount);
	}
#if (defined DEBUG_APP)
	pr_debug("Code analyzed result:");
	for(int i=0; i<str_array.amount;i++) {
		for(int j=0; j<str_get(i)->level; j++) {
			printf(" ");// print level
		}
		pr_info("%s\t:\t<%s>\t<%s> \t conv <%s>\t type <%s> %s", str_get_inst(i),
			lex_to_str(str_get(i)->lext), syn_to_str(str_get(i)->synt),
			type_to_str(str_get(i)->conv_to),
			type_to_str(str_get(i)->ctype),
			(str_get(i)->array)?" is array":""
			);
	}
#endif
	init_prelim();
	preliminary_gen();
	prelim_print_debug();
	pr_debug("assembly generation");
	gen_nasm();
	return 0;
};


