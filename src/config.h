#ifndef CALC_CONFIG
#define CALC_CONFIG

#include "./automata.h"
#include <iostream>

using namespace std;

FSM_STATES(START, LPAREN, RPAREN, MUL, DIV, ADD, SUB, NUM, XOR, OR, AND, NOT, LT, GT, LSHIFT, RSHIFT)

FSM_ALPHABET("*/+-0123456789()^|&!<>");

FSM_START_STATE(START)

FSM_ACCEPT_STATES(LPAREN, RPAREN, MUL, DIV, ADD, SUB, NUM, XOR, OR, AND, NOT, LT, GT, LSHIFT, RSHIFT)

FSM_TRANSITIONS(
    addIO(START, '(', LPAREN);
    addIO(START, ')', RPAREN);
    addIO(START, '*', MUL);
    addIO(START, '/', DIV);
    addIO(START, '+', ADD);
    addIO(START, '-', SUB);
    addIO(START, '^', XOR);
    addIO(START, '|', OR);
    addIO(START, '&', AND);
    addIO(START, '!', NOT);
    addIO(START, '<', LT);
    addIO(START, '>', GT);

    for (int i = '0'; i < '9'; i++)
    {
      addIO(START, i, NUM);
      addIO(NUM, i, NUM);
    }

    addIO(LT, '<', LSHIFT);
    addIO(GT, '>', RSHIFT);)
#endif