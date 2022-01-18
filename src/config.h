#ifndef CALC_CONFIG
#define CALC_CONFIG

#include "./automata.h"
#include <iostream>

using namespace std;

FSM_STATES(
    START,
    LPAREN,
    RPAREN,
    MUL,
    DIV,
    ADD,
    SUB,
    NUM,
    XOR,
    OR,
    AND,
    NOT,
    LT,
    GT,
    LSHIFT,
    RSHIFT)

FSM_ALPHABET('*', '/', '+', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '(', ')', '^', '|', '&', '!', '<', '>')

FSM_START_STATE(START)

FSM_ACCEPT_STATES(
  {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"}, {MUL, "MUL"}, {DIV, "DIV"}, {ADD, "ADD"}, {SUB, "SUB"}, {NUM, "NUM"}, {XOR, "XOR"},
  {OR, "OR"}, {AND, "AND"}, {NOT, "NOT"}, {LT, "LESS_THAN"}, {GT, "GREATER_THAN"}, {LSHIFT, "LEFT_SHIFT"}, {RSHIFT, "RIGHT_SHIFT"}
)

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
    addIO(GT, '>', RSHIFT);
)
#endif