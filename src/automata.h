#ifndef AUTOMATA
#define AUTOMATA

#include <exception>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template <typename STATES, int STATE_COUNT>
class automata
{
  private:
  //STATES PROVIDED VIA TEMPLATE
  unordered_set<char> alphabet;
  STATES startState;
  unordered_set<STATES> acceptStates;
  STATES current;
  unordered_map<STATES, string> tokenMap;

  struct transition_hash
  {
    std::size_t operator()(const std::pair<STATES, char> &p) const
    {
      return std::hash<size_t>{}(p.second * STATE_COUNT + p.first);
    }
  };

  protected:
  unordered_map<pair<STATES, char>, STATES, transition_hash> tFunc;
  virtual void populateTTable() = 0;
  void addIO(STATES current, char in, STATES newState)
  {
    tFunc.insert(make_pair(make_pair(current, in), newState));
  }

  public:
  template <size_t N, size_t M>
  automata(const char (&alphabet)[N], STATES startState, const pair<STATES, string> (&tokenMap)[M])
  {
    this->alphabet = unordered_set<char>(begin(alphabet), end(alphabet));
    this->startState = startState;
    this->current = startState;

    this->acceptStates = unordered_set<STATES>();

    for (auto t = begin(tokenMap); t < end(tokenMap); t++)
    {
      this->acceptStates.insert((*t).first);
    }

    this->tokenMap = unordered_map<STATES, string>(begin(tokenMap), end(tokenMap));

    this->tFunc = unordered_map<pair<STATES, char>, STATES, transition_hash>();
  }

  bool processInput(string in)
  {
    string::iterator it;
    current = startState;
    for (it = in.begin(); it != in.end(); it++)
    {
      if (!alphabet.count(*it)) throw char_not_in_lang(*it);
      try
      {
        current = tFunc.at(make_pair(current, *it));
      }
      catch (out_of_range &e)
      {
        throw no_transition(*it, current);
      }
    }
    if (acceptStates.count(current)) return true;
    return false;
  };

  bool processSingle(char in)
  {
    if (!alphabet.count(in)) throw char_not_in_lang(in);
    try
    {
      current = tFunc.at(make_pair(current, in));
    }
    catch (out_of_range &e)
    {
      throw no_transition(in, current);
    }
    if (acceptStates.count(current)) return true;
    return false;
  }

  void reset()
  {
    current = startState;
  }

  STATES getStart()
  {
    return startState;
  }

  STATES getCurrent()
  {
    return current;
  }

  bool isValid()
  {
    return acceptStates.count(current);
  }

  unordered_map<STATES, string> getTokenMap()
  {
    return tokenMap;
  }

  class no_transition : public exception
  {
private:
    string msg;

public:
    no_transition(char in, STATES current) noexcept
    {
      ostringstream oss;
      oss << "No configured transition from state number " << current << " given input " << in;
      msg = oss.str();
    }
    const char *what() const noexcept override
    {
      return msg.c_str();
    }
  };

  class char_not_in_lang : public exception
  {
private:
    string msg;

public:
    char_not_in_lang(char in) noexcept
    {
      ostringstream oss;
      oss << "The character [ " << in << " ] is not in the specified alphabet ";
      msg = oss.str();
    }
    const char *what() const noexcept
    {
      return msg.c_str();
    }
  };
};

#define FSM_STATES(...) enum FSM_States \
{                                       \
  __VA_ARGS__,                          \
  META_COUNT                            \
};

#define FSM_ALPHABET(...) const char ALPHABET[] = {__VA_ARGS__};

#define FSM_START_STATE(STATE) const FSM_States START_STATE = STATE;

#define FSM_ACCEPT_STATES(...) const pair<FSM_States, string> ACCEPT_NAMED_STATES[] = {__VA_ARGS__};

#define FSM_TRANSITIONS(code)                                                                     \
  class FSM_automaton : public automata<FSM_States, FSM_States::META_COUNT>                       \
  {                                                                                               \
  protected:                                                                                      \
    void populateTTable(){                                                                        \
        code};                                                                                    \
                                                                                                  \
  public:                                                                                         \
    FSM_automaton() : automata(ALPHABET, START_STATE, ACCEPT_NAMED_STATES) { populateTTable(); }; \
  };

#endif