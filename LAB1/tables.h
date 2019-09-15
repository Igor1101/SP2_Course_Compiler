/*
 * tables.h
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#ifndef LAB1_TABLES_H_
#define LAB1_TABLES_H_

typedef struct {
	char* inst;
	int max_coincidence;
}keys_t;

typedef struct {
	char* inst;
	int ch_coincidence;
	int byte_coincidence;
}str_t;

typedef struct {
	str_t*instcs;
	int amount;
}str_array_t;

extern str_array_t str_array;
extern keys_t key;

int str_add(char*str);
char* str_get_inst(int index);
str_t* str_get(int index);
void str_array_remove(void);
void str_get_max(void(*func)(int));
void str_max_update(void);
void str_coincidence(int index);
char* str_alloc(char* str);
void str_free(char*str);
void key_set(char* str);

#endif /* LAB1_TABLES_H_ */
