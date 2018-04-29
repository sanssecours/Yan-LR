// Some parts of this grammar were ported from the
// [ANTLR Python 3 parser](https://github.com/bkiers/python3-parser)
grammar Test;

@header
{
#include <iostream>
#include <memory>
#include <regex>
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

unique_ptr<CommonToken> commonToken(int type, string text, size_t start,
                                    size_t stop) {
  std::unique_ptr<CommonToken> token(
      new CommonToken(make_pair(this, _input), type, DEFAULT_TOKEN_CHANNEL,
                      start, stop));
  return token;
}
}

nodes : node+ EOF ;
node : (ID | NEWLINE | SPACES) ;

NEWLINE : ( '\r'? '\n' ) SPACES? {
  {
    string newLine = regex_replace(this->getText(), std::regex("[^\r\n]"), "");
    string spaces = regex_replace(this->getText(), std::regex("[\r\n]"), "");
    size_t last = getCharIndex() - 1;
    emit(std::move(
        commonToken(NEWLINE, newLine, last - this->getText().length() + 1,
                    last - spaces.length())));
  }
};
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
