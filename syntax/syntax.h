/*
 * analyzer.h
 *
 *  Created on: Oct 9, 2019
 *      Author: igor
 */

#ifndef SYNTAX_SYNTAX_H_
#define SYNTAX_SYNTAX_H_

#define MAX_DEPTH 100
typedef enum {
	S_NOTDEFINED,
	S_OPERAT_RELATION,
	S_OPERAT_ARITHMETIC,
	S_OPERAT_UNARY,
	S_OPERAT_BINARY,
	S_OPERAT_BINARY_UNEXPECTED,
	S_OPERAT_UNARY_UNEXPECTED,
	S_OPERAT_ASSIGNMENT,
	S_OPERAT_ASSIGNMENT_UNEXPECTED,
	S_OPERAT_LOGIC,
	S_OPERAT_BITWISE,
	S_OPERAT_IF,
	S_OPERAT_SWITCH_CASE,
	S_OPERAT_UNKNOWN,
	S_ID_FUNCTION,
	S_FUNC_BRACE_OPEN,
	S_FUNC_BRACE_CLOSE,
	S_EXPECTED_FUNC_BRACE_CLOSE,
	S_EXPECTED_PARAM_DELIMITER,
	S_VARIABLE,
	S_ID_PARAM,
	S_CONST_PARAM,
	S_CONST_PARAM_UNEXPECTED,
	S_CONST,
	S_DEL_PARAM,
	S_DEL,
	S_DEL_EXPECTED,
	S_ID_VARIABLE,
	S_ID_VARIABLE_UNEXPECTED,
	S_ID_UNDEFINED
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
int syn_analyze(void);
int close_brace(int num);
int open_brace(int num);
int push(syn_t op);
syn_t pop(void);
void init_syn_analyzer(void);
int process_ident(int num, int level, bool maybeparam, bool inside_expr);
int process_expression(int num, int level, bool inside_expr);
char* syn_to_str(syn_t t);
int process_function(int num, int level);
int next_delimiter(int num, int level, bool param);
int next_closing_brace(int num, int level);
int is_delimiter_next_expected(int num, int level);



#endif /* SYNTAX_SYNTAX_H_ */
