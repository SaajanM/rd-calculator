#ifndef AUTOMATA
#define AUTOMATA

#include <exception>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
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
  automata(const char (&alphabet)[N], STATES startState, const pair<STATES, string_view> (&tokenMap)[M])
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

template <size_t N>
struct ALPH
{
  constexpr ALPH(const char (&alph)[N]) : letters()
  {
    string_view alphV = alph;
    for (size_t i = 0; i < N - 1; i++)
    {
      letters[i] = *(alphV.data() + i);
    }
  };
  char letters[N - 1];
  constexpr static size_t size = N - 1;
};

constexpr size_t SPLIT(char const *str, char sep)
{
  std::size_t ret{1u};

  while (*str)
    if (sep == *str++)
      ++ret;

  return ret;
}

constexpr size_t LENGTH(string_view a){
  return a.size()+1;
}

template <size_t N, size_t M,typename ENUMTYPE, const ENUMTYPE ... enums>
struct ENUM_MAPPER
{
  constexpr ENUM_MAPPER(const char (&enumstr)[N]) : emap(){
    size_t start = 0, end = 0;

    str = enumstr;
    const size_t count = sizeof...(enums);

    array<ENUMTYPE,count> named_enums = {enums...};

    for (size_t i = 0; i < N && end != string_view::npos; i++)
    {
        end = str.find_first_of(',', start);
        emap[i].first = named_enums[i];
        emap[i].second = string_view(str.substr(start, end - start));
        start = end + 2;
    }
  };
  string_view str;
  pair<ENUMTYPE, string_view> emap[M];
};

#define FSM_ALPHABET(...) constexpr ALPH ALPHABET(__VA_ARGS__);

#define FSM_ACCEPT_STATES(...) constexpr ENUM_MAPPER ACCEPT_NAMED_STATES = ENUM_MAPPER<LENGTH(#__VA_ARGS__), SPLIT(#__VA_ARGS__ , ','), FSM_States, __VA_ARGS__>(#__VA_ARGS__);

#define FSM_START_STATE(STATE) constexpr FSM_States START_STATE = STATE;

#define FSM_TRANSITIONS(code)                                                                             \
  class FSM_automaton : public automata<FSM_States, FSM_States::META_COUNT>                               \
  {                                                                                                       \
protected:                                                                                                \
    void populateTTable(){                                                                                \
        code};                                                                                            \
                                                                                                          \
public:                                                                                                   \
    FSM_automaton() : automata(ALPHABET.letters, START_STATE, ACCEPT_NAMED_STATES.emap) { populateTTable(); }; \
  };

#endif