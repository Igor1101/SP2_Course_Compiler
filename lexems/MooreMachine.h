/*
 * MooreMachine.h
 *
 *  Created on: Sep 22, 2019
 *      Author: igor
 */

#ifndef TREES_MOOREMACHINE_H_
#define TREES_MOOREMACHINE_H_

typedef enum {
    Eu,             // UNKNOWN OBJECT
    S0,             // ANY SEPARATOR
    S1g,            // SIGN OF CONSTANT
    S1c,            //DIGIT OF INTEGER NUMBER(CONSTANT)
    S2c,            //NUMBER WITH DOT
    S1e,            //EXPONENT
    S1q,            //SIGN
    S1p,            //DEC DIGIT OF EXPONENT
    S1n,            //NAME
    S1s,            //SYMBOL OR SYMBOL STRING
    S1t,            //ENCODED SYMBOL
    S2s,            //SYMBOL OR SYMBOL STRING END
    SB,             //NUMBER BASE POINTER "0"
    SBN,            //BASE NUMBER
    SBNB,           //NUMBER BODY
    S2,             //START GROUP SEPARATOR
    S3,             //NEXT GROUP SEPARATOR
    Scr,            // COMMENT
    Scl,            // SEPARATED COMMENT
    Ec,             // ERROR IN CONSTANT
    Ep,             // ERROR IN DOT CONSTANT
    Eq,             // ERROR IN EXP CONSTANT
    En,             // ERROR IN NAME(LETTER ETC)
    Es,             // ERROR IN TERM
    Eo              // UNACCEPTABLE COMBINATION IN OPERATION
}MOORE_STATE;

typedef enum{
    SIG_DIGIT,  // DECIMAL DIG
    SIG_EXP,    // EXPONENT
    SIG_HEX,    // HEX DIGIT
    SIG_CONST,  // DETERM CONSTANT
    SIG_NAME,   // ONLY NAME ACCEPTABLE LETTER
    SIG_STRLIM, // STRING\CONST LIMITATION
    SIG_STRBEG, // BEGINNING OF STRING DECODES
    SIG_NC,     // NOT CLASSIFIED SYMBOL
    SIG_DOT,    // DOT AS SEPARATOR AND AS CONSTANT LETTER
    SIG_SIGN,   // NUMBER OR EXPONENT SIGN
    SIG_EMPTY,  // ADDITIONAL SEPARATOR AS TAB, ' '
    SIG_SEPOP,  // SINGLE OPERATION SEPARATOR
    SIG_SEPGROUP, // GROUP SEPARATOR
    SIG_SEPLST, // LIST SEPARATOR
    SIG_OPENBR, // OPEN BRACE
    SIG_CLOSEDBR, // CLOSED BRACE
    SIG_LTRCODE = 16// AN INDICATION OF POSSIBILITY TO ENCODE
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
