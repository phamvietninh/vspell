#ifndef __TOKENIZE_H__
#define __TOKENIZE_H__

#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif

struct Token
{
  bool is_token;
  std::string value;

  Token(bool _is_token, const std::string &_value):
    is_token(_is_token),
    value(_value) 
  {}
};

typedef std::vector<Token> Tokens;

bool tokenize(const std::string& str,Tokens &tokens);

#endif
