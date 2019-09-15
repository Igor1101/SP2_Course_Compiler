/*
 * tables.c
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <LAB1/ch_utf8.h>
#include "tables.h"

keys_t key = {
		.inst = NULL,
		.max_coincidence = 0
};

str_array_t str_array = {
		.instcs = NULL,
		.amount = 0
};

int str_add(char*str)
{
	int strindex = str_array.amount;
	if(str_array.instcs == NULL) {
		str_array.instcs = (str_t*)calloc(1,sizeof(str_t));
		str_array.amount = 1;
	} else {
		str_array.amount++;
		str_array.instcs =
				reallocarray(str_array.instcs, str_array.amount, sizeof(str_t));
		if(str_array.instcs == NULL) {
			pr_err("array alloc failed");
			exit(-1);
		}
	}
	str_array.instcs[strindex].inst = str_alloc(str);
	/* return index */
	return strindex;
}

char* str_alloc(char* str)
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

void str_free(char*str)
{
	if(str == NULL)
		return;
	free(str);
	str = NULL;
}

char* str_get_inst(int index)
{
	if(str_array.amount <= index) {
		pr_err("str_get: out of bounds");
		return NULL;
	}
	return str_array.instcs[index].inst;
}

str_t* str_get(int index)
{
	if(str_array.amount <= index) {
		pr_err("str_get: out of bounds");
		return NULL;
	}
	return &str_array.instcs[index];
}

void str_array_remove(void)
{
	if(str_array.instcs == NULL)
		return;
	for(int i=0; i<str_array.amount; i++) {
		str_free(str_array.instcs[i].inst);
	}
	free(str_array.instcs);
	str_array.instcs = NULL;
	str_array.amount = 0;
}

void str_coincidence(int index)
{
	if(key.inst == NULL) {
		str_array.instcs[index].ch_coincidence = 0;
		return;
	}
	str_array.instcs[index].ch_coincidence = ch_utf_strcmp(str_array.instcs[index].inst, key.inst);
}

void str_max_update(void)
{
	int max = 0;
	for(int i=0; i<str_array.amount; i++)
		str_coincidence(i);
	/* find max here */
	for(int i=0; i<str_array.amount; i++) {
		if(str_array.instcs[i].ch_coincidence > max)
			max = str_array.instcs[i].ch_coincidence;
	}
	key.max_coincidence = max;
	pr_debug("key max coincidence=%d", key.max_coincidence);
}

void str_get_max(void(*func)(int))
{
	if(func == NULL)
		return;
	/* find max here */
	for(int i=0; i<str_array.amount; i++) {
		if(key.max_coincidence == str_array.instcs[i].ch_coincidence)
			func(i);
	}
}

void key_set(char* str)
{
	str_free(key.inst);
	key.inst = str_alloc(str);
}

void str_del(int index)
{
	str_free(str_get_inst(index));
	if(str_array.amount == 0)
		return;
	if(str_array.amount <= index)
		return;
	for(int i=index+1; i<str_array.amount; i++) {
		str_array.instcs[i-1] = str_array.instcs[i];
	}
	str_array.amount--;
	str_array.instcs = reallocarray(str_array.instcs, str_array.amount, sizeof(str_t));
}
