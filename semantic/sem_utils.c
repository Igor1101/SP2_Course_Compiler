/*
 * sem_utils.c
 *
 *  Created on: Nov 6, 2019
 *      Author: igor
 */

#include <defs.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <lexems/tables.h>
#include "tables_sem.h"
#include "sem.h"

void set_err(int num, char* format, ...)
{
#define MAX_SERRSZ 1000
	va_list argp;
	va_start(argp, format);
	char* output = malloc(MAX_SERRSZ);
	if(output == NULL) {
		pr_err("malloc err");
		return ;
	}
	vsnprintf(output, MAX_SERRSZ, format, argp);
	str_get(num)->sem_err=true;
	str_get(num)->sem_inst_err = str_alloc(output);
	free(output);
	va_end(argp);
#undef MAX_SERRSZ
}

void set_noerr(int num)
{
	str_get(num)->sem_err=false;
	str_free(str_get(num)->sem_inst_err);
}

ctypes_t str_to_type(char*str)
{
	if(!strcmp(str, "char")) return C_CHAR_T;
	if(!strcmp(str, "short")) return C_SHORT_T;
	if(!strcmp(str, "int")) return C_INT_T;
	if(!strcmp(str, "unsigned")) return C_UNSIGNED_T;
	if(!strcmp(str, "signed")) return C_SIGNED_T;
	if(!strcmp(str, "long")) return C_LONG_T;
	if(!strcmp(str, "float")) return C_FLOAT_T;
	if(!strcmp(str, "double")) return C_DOUBLE_T;
	return C_UKNOWN;
}

char* type_to_str(ctypes_t t)
{
	switch(t) {
	case C_CHAR_T:
		return "char";
	case C_SHORT_T:
		return "short";
	case C_INT_T:
		return "int";
	case C_UNSIGNED_T:
		return "unsigned";
	case C_SIGNED_T:
		return "signed";
	case C_LONG_T:
		return "long";
	case C_FLOAT_T:
		return "float";
	case C_DOUBLE_T:
		return "double";
	case C_UKNOWN:
		return "unknown";
	}
}

ctype_conv_t type0_to_type1_acc(ctypes_t t0, ctypes_t t1)
{
	switch(t0) {
	case C_CHAR_T:
		switch(t1) {
		case C_CHAR_T:
			return ACCEPT;
		case C_SIGNED_T:
		case C_INT_T:
		case C_LONG_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_SHORT_T:
		switch(t1) {
		case C_SHORT_T:
			return ACCEPT;
		case C_INT_T:
		case C_LONG_T:
		case C_SIGNED_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_INT_T:
		switch(t1) {
		case C_INT_T:
		case C_SIGNED_T:
			return ACCEPT;
		case C_UNSIGNED_T:
		case C_LONG_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_SIGNED_T:
		switch(t1) {
		case C_SIGNED_T:
		case C_INT_T:
			return ACCEPT;
		case C_LONG_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_LONG_T:
		switch(t1) {
		case C_LONG_T:
			return ACCEPT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_UNSIGNED_T:
		switch(t1) {
		case C_UNSIGNED_T:
			return ACCEPT;
		case C_INT_T:
		case C_LONG_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_FLOAT_T:
		switch(t1) {
		case C_FLOAT_T:
			return ACCEPT;
		case C_DOUBLE_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	case C_DOUBLE_T:
		switch(t1) {
		case C_DOUBLE_T:
			return ACCEPT;
		case C_FLOAT_T:
			return CONVERT;
		default:
			return NOTACCEPT;
		}
		break;
	}
	return NOTACCEPT;
}

int sem_err_amount(void)
{
	int err_amount=0;
	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->sem_err)
			err_amount++;
	}
	return err_amount;
}

void sem_printf(void)
{
	pr_info("sem output:");
	char *arr = malloc(1000);
	size_t *sz = malloc(str_array.amount*sizeof(size_t));
	size_t sz_cur = 0;

	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->sem_err) {
			printf(COLOR_RED " %s" COLOR_DEF, str_get_inst(i));
		} else {
			printf(COLOR_GREEN " %s" COLOR_DEF, str_get_inst(i));
		}
		sprintf(arr, " %s" , str_get_inst(i));
		sz[i] = strlen(arr) + sz_cur;
		sz_cur = sz[i];
	}
	puts("");
	for(int i=0; i<str_array.amount;i++) {
		if(str_get(i)->sem_err) {
			for(int j=0; j<sz[i]; j++)
				putchar(' ');
			pr_info("^ %s", str_get(i)->sem_inst_err);
		}
	}
	free(arr);
	free(sz);
}
