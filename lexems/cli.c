/*
 * cli.c
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */

#include <stdio.h>
#include <string.h>
#include <lexems/utf8.h>
#include <lexems/ch_utf8.h>
#include "cli.h"

char cli_line[1000];
char cli_word[CLI_WORDS][100];

static char* find_notspace(char* str);
static char* find_space(char* str);

void cli_get_words(void)
{
    for (char* p = cli_line; (p = strchr(p, '\n')); ++p)
        *p = '\0';
    for (char* p = cli_line; (p = strchr(p, '\r')); ++p)
        *p = '\0';
    memset(cli_word, 0, sizeof cli_word);
    int w_num = 0;
    char* w_n_start = find_notspace(cli_line);
    char* w_n_1_st;
    for(w_num=0; w_num<CLI_WORDS; w_num++) {
        w_n_1_st = find_notspace(find_space(w_n_start));
        if(find_space(w_n_start) != NULL) {
            *find_space(w_n_start) = '\0';
            //int i=0;
            //ch_t ch = u8_nextchar(w_n_start-1, &i);
            //u8_toutf8(cli_word[w_num], sizeof cli_word[w_num], &ch, sizeof ch);
            //cli_word[w_num][0] = 0xEF;
            //cli_word[w_num][1] = 0xBB;
            //cli_word[w_num][2] = 0xBF;
            //u8_strncpy(cli_word[w_num] + u8_offset(cli_word[w_num], 1),
            //		w_n_start+u8_offset(w_n_start, 1), sizeof cli_word[w_num]);
            u8_strncpy_part(cli_word[w_num], w_n_start, sizeof cli_word[w_num]);
        }
        w_n_start = w_n_1_st;
    }
}

static char* find_notspace(char* str)
{
	int i=0;
    if(str == NULL)
        return NULL;
    while(u8_nextchar(str, &i) == ' ');
    return str+i-1;
}

static char* find_space(char* str)
{
	int i=0;
    if(str == NULL)
        return NULL;
    while(u8_nextchar(str, &i) != ' ') {
    	u8_dec(str, &i);
        if(u8_nextchar(str, &i) == '\0') {
            return str+i-1;
        }
    }
    return str+i-1;
}

