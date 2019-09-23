/*
 * cli.h
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#ifndef LAB1_CLI_H_
#define LAB1_CLI_H_
#include <string.h>

#define CLI_IS(X) !strcmp(cli_word[0], X)
#define ARG_0 cli_word[0]
#define ARG_1 cli_word[1]
#define ARG_2 cli_word[2]
#define ARG_3 cli_word[3]
#define ARG_4 cli_word[4]
#define ARG_5 cli_word[5]
#define CLI_WORDS 6

extern char cli_line[1000];
extern char cli_word[CLI_WORDS][100];


void cli_get_words(void);



#endif /* LAB1_CLI_H_ */
