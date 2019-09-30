/*
 * tables.h
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#ifndef LAB1_TABLES_H_
#define LAB1_TABLES_H_
#include <lexems/lex.h>

typedef struct {
	char* inst;
	int max_coincidence;
}keys_t;

typedef struct {
	char* inst;
	int ch_coincidence;
	lexem_t lext;
}str_t;

typedef struct {
	str_t*instcs;
	int amount;
}str_array_t;

extern str_array_t str_array;
extern keys_t key;

int str_add(char*str);
void str_del(int index);
void str_modify(int index, char* new);
void str_modify_max(char* new);
char* str_get_inst(int index);
str_t* str_get(int index);
void str_array_remove(void);
void str_get_max(void(*func)(int));
void str_get_max_bin(void(*func)(int));
void str_max_update(void);
void str_coincidence(int index);
char* str_alloc(char* str);
void str_free(char*str);
void key_set(char* str);
void str_sort(void);
void str_insert(int index, char* str);
void str_del_max(void);

#endif /* LAB1_TABLES_H_ */
