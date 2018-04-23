// Some parts of this grammar were ported from the
// [ANTLR Python 3 parser](https://github.com/bkiers/python3-parser)
grammar Test;

@header
{
#include <iostream>
#include <memory>
#include <stack>
}

@postinclude
{
using namespace std;
using namespace antlr4;
}

@lexer::members
{
private:
  stack<size_t> indents;

unique_ptr<CommonToken> commonToken(int type, string text) {
  int stop = getCharIndex() - 1;
  int start = text.empty() ? stop : stop - text.length() + 1;
  std::unique_ptr<CommonToken> token(
      new CommonToken(make_pair(this, _input), type, DEFAULT_TOKEN_CHANNEL,
                      start, stop));
  return token;
}
}

nodes : node+ EOF ;
node : (ID | NEWLINE | SPACES) ;

NEWLINE : '\n' SPACES? {
  emit(commonToken(NEWLINE, "\n"));
};
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
