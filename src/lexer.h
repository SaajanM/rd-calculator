#ifndef LEXER
#define LEXER

#include "./automata.h"
#include <iostream>
#include <unordered_map>
#include <vector>

struct lex_pair
{
  string token;
  string contents;
};

template <typename STATES, int STATE_COUNT>
class lexer
{
  private:
  automata<STATES, STATE_COUNT> *fsm = nullptr;
  string::iterator begin;
  string::iterator current;
  vector<lex_pair> lexicon;
  size_t lexerPos;
  unordered_map<STATES, string> tokenMap;
  char delim;

  void assignToken(string::iterator begin, string::iterator end)
  {
    STATES currentState = fsm->getCurrent();
    string foundMapping = tokenMap.at(currentState);
    lex_pair lexeme = {foundMapping, string(begin, end)};
    lexicon.push_back(lexeme);
  }

  void lexInput(string in)
  {
    fsm->reset();

    begin = in.begin();
    current = in.begin();

    string::iterator lastLabeled = in.begin();
    bool hasLastLabeled = false;

    while (current != in.end())
    {
      if (*current == delim)
      {
        if (fsm->isValid())
        {
          assignToken(begin, current);
        }

        current++;
        begin = current;
        lastLabeled = current;
        hasLastLabeled = false;
        fsm->reset();
        continue;
      }

      try
      {
        fsm->processSingle(*current);
        if (fsm->isValid())
        {
          lastLabeled = current;
          hasLastLabeled = true;
        }
      }
      catch (typename automata<STATES, STATE_COUNT>::no_transition &e)
      {
        if (!hasLastLabeled) throw invalid_argument("Can't read input");
        current = lastLabeled + 1;
        assignToken(begin, current);
        begin = current;
        fsm->reset();
        continue;
      }

      current++;
    }

    if (fsm->isValid())
    {
      assignToken(begin, current);
    }
  }

  public:
  lexer(automata<STATES, STATE_COUNT> *fsm, const char delim = ' ')
  {
    this->fsm = fsm;
    this->delim = delim;
    this->tokenMap = fsm->getTokenMap();
    this->lexicon = vector<lex_pair>();
    this->lexerPos = -1;
  }

  void processString(string input){
    try{
      lexInput(input);
    }catch(typename automata<STATES,STATE_COUNT>::char_not_in_lang &e){
      cout << e.what() << endl;
    }
  }

  lex_pair getToken(){
    lex_pair res = peek();
    if(res.token!="EOF"){
      lexerPos++;
    }
    return res;
  }

  lex_pair peek(){
    if(lexerPos == lexicon.size()-1){
      return (lex_pair){"EOF",""};
    }else{
      return (lexicon.at(lexerPos+1));
    }
  }
};

#endif