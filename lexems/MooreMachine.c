/*
 * MooreMachine.c
 *
 *  Created on: Sep 22, 2019
 *      Author: igor
 */

#include <string.h>
#include "MooreMachine.h"
MOORE_STATE graph[Eo+1][SIG_LTRCODE+1] = {
	//DIG EXP HEX CONST NAME STRLIM STRBEG NC DOT SIGN
	//EMPTY SEPOP SPGROUP SEPLST OPENBR CLOSEBR LTRCODE
		// Eu
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu },
	{ Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu, Eu }
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
	return SIG_EMPTY;
}

MOORE_SIGNAL moore_char_to_sig(unsigned num)
{
}

char* moore_sig_to_str(MOORE_SIGNAL sig)
{
	char*str;
	switch(sig) {
	case SIG_CLOSEDBR:
		str = "CLOSEDBR";
		break;
	case SIG_CONST:
		str = "CONST";
		break;
	case SIG_DIGIT:
		str = "DIGIT";
		break;
	case SIG_DOT:
		str = "DOT";
		break;
	case SIG_EMPTY:
		str = "EMPTY";
		break;
	case SIG_EXP:
		str = "EXP";
		break;
	case SIG_HEX:
		str = "HEX";
		break;
	case SIG_LTRCODE:
		str = "LTRCODE";
		break;
	default:
		str = "UNKNOWN SIGNAL";
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

void moore_reset(void)
{
	cur_state = S0;
}
