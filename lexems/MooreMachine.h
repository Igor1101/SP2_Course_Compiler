/*
 * MooreMachine.h
 *
 *  Created on: Sep 22, 2019
 *      Author: igor
 */

#ifndef TREES_MOOREMACHINE_H_
#define TREES_MOOREMACHINE_H_

typedef enum {
	S0, S1, S2, S3,
	S4, S5, S6, Se
}MOORE_STATE;

typedef enum{
	SIG_DLM,
	SIG_CFR,
	SIG_LTR,
	SIG_E
}MOORE_SIGNAL;

MOORE_STATE moore_next_state(MOORE_STATE st, MOORE_SIGNAL sig);
MOORE_STATE moore_next_state_move(MOORE_SIGNAL sig);
MOORE_SIGNAL moore_str_to_sig(char*sig);
char* moore_sig_to_str(MOORE_SIGNAL sig);
char* moore_state_to_str(MOORE_STATE st);
MOORE_STATE moore_getstate(void);
MOORE_STATE moore_next_state_move_str(char* sig_s);
void moore_reset(void);


#endif /* TREES_MOOREMACHINE_H_ */
