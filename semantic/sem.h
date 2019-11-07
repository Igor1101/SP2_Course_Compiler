/*
 * sem.h
 *
 *  Created on: Nov 5, 2019
 *      Author: igor
 */

#ifndef SEMANTIC_SEM_H_
#define SEMANTIC_SEM_H_
#include "tables_sem.h"
/* sem utils */
ctype_conv_t type0_to_type1_acc(ctypes_t t0, ctypes_t t1);
void set_err(int num, char* format, ...);
ctypes_t str_to_type(char*str);
char* type_to_str(ctypes_t t);
void sem_printf(void);
int sem_err_amount(void);
void set_err_type(int num, ctypes_t from, ctypes_t to);
void sem_database_print(void);
void set_err_already_decl(int num);
void set_err_undeclared_used(int num);

/* analyzer */
int sem_analyze(void);



#endif /* SEMANTIC_SEM_H_ */
