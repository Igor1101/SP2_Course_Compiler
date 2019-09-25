/*
 * MooreMachine.c
 *
 *  Created on: Sep 22, 2019
 *      Author: igor
 */

#include <string.h>
#include "MooreMachine.h"
MOORE_STATE graph[Se+1][SIG_E+1] = {
		/* DLM,CFR,LTR, SIG_E */
		{ S1, S1, S1, S0 },// S0
		{ S2, S2, S2, S1 },// S1
		{ S3, S3, S3, S2 },// S2
		{ Se, S4, S4, S3 },// S3
		{ S5, S5, S4, S4 },// S4
		{ S6, S3, S6, S5 },// S5
		{ Se, Se, Se, S6 },// S6
		{ Se, Se, Se, Se }// S7
};
static MOORE_STATE cur_state;

MOORE_STATE moore_next_state(MOORE_STATE st, MOORE_SIGNAL sig)
{
	return graph[st][sig];
}

MOORE_STATE moore_next_state_move(MOORE_SIGNAL sig)
{
	cur_state = moore_next_state(cur_state, sig);
	return cur_state;
}

MOORE_SIGNAL moore_str_to_sig(char*sig)
{
	if(sig == NULL)
		return SIG_E;
	if(!strcmp(sig, "cfr"))
		return SIG_CFR;
	if(!strcmp(sig, "dlm"))
		return SIG_DLM;
	if(!strcmp(sig, "ltr"))
		return SIG_LTR;
	return SIG_E;
}

char* moore_sig_to_str(MOORE_SIGNAL sig)
{
	char*str;
	switch(sig) {
	case SIG_CFR:
		str = "cfr";
		break;
	case SIG_DLM:
		str = "dlm";
		break;
	case SIG_LTR:
		str = "ltr";
		break;
	case SIG_E:
		str = "e";
		break;
	default:
		str = "e";
	}
	return str;
}

char* moore_state_to_str(MOORE_STATE st)
{
	char*str;
	switch(st) {
	case S0:
		str = "S0";
		break;
	case S1:
		str = "S1";
		break;
	case S2:
		str = "S2";
		break;
	case S3:
		str = "S3";
		break;
	case S4:
		str = "S4";
		break;
	case S5:
		str = "S5";
		break;
	case S6:
		str = "S6";
		break;
	case Se:
		str = "Se";
		break;
	default:
		str = "Undefined";
	}
	return str;
}

MOORE_STATE moore_getstate(void)
{
	return cur_state;
}

MOORE_STATE moore_next_state_move_str(char* sig_s)
{
	MOORE_SIGNAL sig = moore_str_to_sig(sig_s);
	return moore_next_state_move(sig);
}
