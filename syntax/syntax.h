/*
 * analyzer.h
 *
 *  Created on: Oct 9, 2019
 *      Author: igor
 */

#ifndef SYNTAX_SYNTAX_H_
#define SYNTAX_SYNTAX_H_

#include <stdbool.h>
#include <lexems/lex.h>
#define MAX_DEPTH 100
typedef enum {
	S_NOTDEFINED = 0,
	S_KEYWORD,
	S_KEYWORD_TYPE,
	S_OPERAT_RELATION,
	S_OPERAT_ARITHMETIC,
	S_OPERAT_UNARY,
	S_OPERAT_BINARY,
	S_OPERAT_ASSIGNMENT,
	S_OPERAT_LOGIC,
	S_OPERAT_BITWISE,
	S_OPERAT_IF,
	S_OPERAT_SWITCH_CASE,
	S_ID_FUNCTION,
	S_FUNC_BRACE_OPEN,
	S_FUNC_BRACE_CLOSE,
	S_BRACE_OPEN,
	S_BRACE_CLOSE,
	S_VARIABLE,
	S_ID_PARAM,
	S_CONST_PARAM,
	S_CONST,
	S_DEL_PARAM,
	S_DEL,
	S_ID_VARIABLE,
	S_ID_UNDEFINED,
	S_CHAR,
	S_STRING,
	S_POINTER
}syn_t;

typedef struct {
	int num;
	int symbols_after_del;
	int curlybrace_nest;
	int parensis_nest;
	int square_braces_nest;
	int nesting;
	bool assignment;
	bool arithmetic;
	syn_t op[MAX_DEPTH];
	int __op_p;
}syn_state_t;
extern syn_state_t st;
/* syn utils */
char* syn_to_str(syn_t t);
void set_synt(int num, syn_t s, int level);
void set_synt_err_unexp(int num, syn_t expected, syn_t unexpected);
void set_synt_err(int num, syn_t exp);
syn_t lex_to_syn(lexem_t l);
void syn_printf(void);

/* syntax analyzer */
int syn_analyze(void);
int close_brace(int num);
int open_brace(int num);
int push(syn_t op);
syn_t pop(void);
void init_syn_analyzer(void);
int process_ident(int num, int level, bool maybeparam, bool inside_expr);
int process_expression(int num, int level, bool inside_expr, bool inside_array);
int process_function(int num, int level);
int process_for_loop(int num, int level);
int next_delimiter(int num, int level, bool param);
int next_closing_brace(int num, int level);
int is_delimiter_next_expected(int num, int level, bool forcenext);
int process_array(int num, int level);



#endif /* SYNTAX_SYNTAX_H_ */
