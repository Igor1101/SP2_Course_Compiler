/*
 * tables.c
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */
#include "tables_sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <lexems/ch_utf8.h>

ident_array_t ident_array = {
		.instcs = NULL,
		.amount = 0
};

int ident_add(char*str, ctypes_t t, bool array)
{
	/* check if it is already declared */
	if(ident_present(str))
		return -1;
	int strindex = ident_array.amount;
	if(ident_array.instcs == NULL) {
		ident_array.instcs = (ident_t*)calloc(1,sizeof(ident_t));
		ident_array.amount = 1;
	} else {
		ident_array.amount++;
		ident_array.instcs =
				reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
		if(ident_array.instcs == NULL) {
			pr_err("array alloc failed");
			exit(-1);
		}
	}
	ident_array.instcs[strindex].inst = ident_alloc(str);
	ident_array.instcs[strindex].type = t;
	ident_array.instcs[strindex].array = array;
	/* return index */
	return strindex;
}

char* ident_alloc(char* str)
{
	size_t sz = strlen(str);
	char* alloc = (char*)calloc(sz, 1);
	if(alloc == NULL) {
		pr_err("out of memory");
		exit(-1);
	}
	strncpy(alloc, str, sz);
	return alloc;
}

void ident_free(char*str)
{
	if(str == NULL)
		return;
	free(str);
	str = NULL;
}

char* ident_get_inst(int index)
{
	if(ident_array.amount <= index) {
		pr_err("ident_get: out of bounds");
		return NULL;
	}
	return ident_array.instcs[index].inst;
}

ident_t* ident_get(int index)
{
	if(ident_array.amount <= index) {
		pr_err("ident_get: out of bounds");
		static ident_t str = {
				.inst = NULL
		};
		return &str;
	}
	return &ident_array.instcs[index];
}

ident_t* ident_get_str(char*str)
{
	for(int i=0; i<ident_array.amount; i++) {
		if(!strcmp(ident_get(i)->inst, str)) {
			return ident_get(i);
		}
	}
	/* not found */
	return NULL;
}

bool ident_set_init(char*str)
{
	ident_t* id = ident_get_str(str);
	if(id == NULL)
		return false;
	id->init = true;
	return true;
}

bool ident_set_used(char*str)
{
	ident_t* id = ident_get_str(str);
	if(id == NULL)
		return false;
	id->used = true;
	return true;
}

bool ident_present(char*str)
{
	if(ident_get_str(str)!=NULL) {
		return true;
	}
	return false;
}

void ident_array_remove(void)
{
	if(ident_array.instcs == NULL)
		return;
	for(int i=0; i<ident_array.amount; i++) {
		ident_free(ident_array.instcs[i].inst);
	}
	free(ident_array.instcs);
	ident_array.instcs = NULL;
	ident_array.amount = 0;
}

void ident_del(int index)
{
	ident_free(ident_get_inst(index));
	if(ident_array.amount == 0)
		return;
	if(ident_array.amount <= index)
		return;
	for(int i=index+1; i<ident_array.amount; i++) {
		ident_array.instcs[i-1] = ident_array.instcs[i];
	}
	ident_array.amount--;
	ident_array.instcs = reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
}

void ident_modify(int index, char* new)
{
	if(ident_array.amount == 0)
		return;
	if(ident_array.amount <= index)
		return;
	ident_free(ident_get_inst(index));
	ident_get(index)->inst = ident_alloc(new);
}

void ident_insert(int index, char* str)
{
	if(ident_array.instcs == NULL) {
		pr_err("cant insert str into empty array");
		return;
	}
	if(index >= ident_array.amount) {
		pr_err("index out of bounds");
		return;
	}
	/* realloc array */
	ident_array.amount++;
	ident_array.instcs =
			reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
	if(ident_array.instcs == NULL) {
		pr_err("array alloc failed");
		exit(-1);
	}
	/* shift all elements higher index to the right */
	for(int i=ident_array.amount - 1; i>index; i--)
		ident_array.instcs[i] = ident_array.instcs[i-1];
	ident_array.instcs[index].inst = ident_alloc(str);
}
