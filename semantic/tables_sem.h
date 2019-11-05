/*
 * tables.h
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#ifndef LAB1_TABLES_H_
#define LAB1_TABLES_H_
#include <stdbool.h>
#include <lexems/lex.h>
#include <syntax/syntax.h>

typedef struct {
	char* inst;
	int max_coincidence;
}keys_t;

typedef struct {
	char* inst;
	char* syn_inst_exp;
	int level;
	lexem_t lext;
	syn_t synt;
	syn_t synexp;
	syn_t synunexp;
	bool syn_err;
	bool lex_err;
}ident_t;

typedef struct {
	ident_t*instcs;
	int amount;
}ident_array_t;

extern ident_array_t ident_array;
extern keys_t key;

int ident_add(char*str, lexem_t lt);
void ident_del(int index);
void ident_modify(int index, char* new);
void ident_modify_max(char* new);
char* ident_get_inst(int index);
ident_t* ident_get(int index);
void ident_array_remove(void);
void ident_get_max(void(*func)(int));
void ident_get_max_bin(void(*func)(int));
void ident_max_update(void);
void ident_coincidence(int index);
char* ident_alloc(char* str);
void ident_free(char*str);
void key_set(char* str);
void ident_sort(void);
void ident_insert(int index, char* str);
void ident_del_max(void);

#endif /* LAB1_TABLES_H_ */
