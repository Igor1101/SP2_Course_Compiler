/*
 * analyzer.h
 *
 *  Created on: Oct 9, 2019
 *      Author: igor
 */

#ifndef SYNTAX_SYNTAX_H_
#define SYNTAX_SYNTAX_H_

typedef struct {
	int symbols_after_del;
	int curlybrace_num;
	int parensis_num;
	int nesting;
	bool assignment;
	bool arithmetic;
}syn_state_t;
extern syn_state_t st;
int analyze(void);



#endif /* SYNTAX_SYNTAX_H_ */
