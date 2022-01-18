#include "./config.h"
#include "./lexer.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
  FSM_automaton a;

  lexer<FSM_States,FSM_States::META_COUNT> l(&a);

  string t;
  getline(cin,t);

  l.processString(t);

  while(l.peek().token!="EOF"){
    cout << l.peek().token << ", " << l.getToken().contents << endl;
  }

  return 0;
}
