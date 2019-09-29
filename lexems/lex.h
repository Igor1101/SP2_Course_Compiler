/*
 * lex.h
 *
 *  Created on: Sep 29, 2019
 *      Author: igor
 */

#ifndef LEXEMS_LEX_H_
#define LEXEMS_LEX_H_

#include <lexems/MooreMachine.h>

bool is_keyword(char* str);
bool is_name(char* str);
bool is_char_in(unsigned ch, const char*str);
MOORE_SIGNAL ch_to_moore_signal(unsigned ch);



#endif /* LEXEMS_LEX_H_ */
