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
ctypes_t str_to_type(char*str);
void sem_printf(void);
int sem_err_amount(void);
void sem_database_print(void);
/* analyzer */
int sem_analyze(void);



#endif /* SEMANTIC_SEM_H_ */
