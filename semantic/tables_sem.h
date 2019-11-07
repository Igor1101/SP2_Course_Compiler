/*
 * tables.h
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#ifndef LAB1_TABLESSEM_H_
#define LAB1_TABLESSEM_H_
#include <stdbool.h>

typedef enum {
	NOTACCEPT,
	ACCEPT,
	CONVERT
} ctype_conv_t;
typedef enum {
	C_UKNOWN=0,
	C_CHAR_T,
	C_SHORT_T,
	C_INT_T,
	C_UNSIGNED_T,
	C_SIGNED_T,
	C_LONG_T,
	C_FLOAT_T,
	C_DOUBLE_T,
}ctypes_t;

typedef struct {
	char* inst;
	ctypes_t type;
	bool init;
	bool used;
	bool array;
}ident_t;

typedef struct {
	ident_t*instcs;
	int amount;
}ident_array_t;

extern ident_array_t ident_array;

int ident_add(char*str, ctypes_t t, bool init);
void ident_del(int index);
void ident_modify(int index, char* new);
void ident_modify_max(char* new);
char* ident_get_inst(int index);
ident_t* ident_get(int index);
ident_t* ident_get_str(char*str);
void ident_array_remove(void);
char* ident_alloc(char* str);
void ident_free(char*str);
void key_set(char* str);
void ident_sort(void);
void ident_insert(int index, char* str);
void ident_del_max(void);
bool ident_present(char*str);
bool ident_set_used(char*str);
bool ident_set_init(char*str);

#endif /* LAB1_TABLESSEM_H_ */
