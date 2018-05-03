// Some parts of this grammar were ported from the
// [ANTLR Python 3 parser](https://github.com/bkiers/python3-parser)
grammar Test;

tokens { INDENT, DEDENT }

@lexer::header
{
#include <iostream>
#include <memory>
#include <regex>
#include <stack>

#include "TestParser.h"
}

@lexer::postinclude
{
using namespace std;
using namespace antlr4;
}

@lexer::members
{
private:

stack<size_t> indents;
list<CommonToken> tokens;

unique_ptr<CommonToken> commonToken(int type, string text, size_t start,
                                    size_t stop) {
  unique_ptr<CommonToken> token(
      new CommonToken(make_pair(this, _input), type, DEFAULT_TOKEN_CHANNEL,
                      start, stop));
  return move(token);
}

public:

void emit(unique_ptr<Token> token) override {
  tokens.push_back(dynamic_cast<CommonToken*>(&*token));
  Lexer::setToken(move(token));
}

unique_ptr<Token> nextToken() override {
  auto next = Lexer::nextToken();
  if (tokens.empty()) {
    return Lexer::nextToken();
  }
  unique_ptr<Token> first(new CommonToken(tokens.front()));
  tokens.pop_front();
  return move(first);
}

}

nodes : node+ EOF ;
node : INDENT? ID NEWLINE;

NEWLINE : ( '\r'? '\n' ) SPACES? {{
  string newLine = regex_replace(this->getText(), regex("[^\r\n]"), "");
  string spaces = regex_replace(this->getText(), regex("[\r\n]"), "");
  size_t last = getCharIndex() - 1;
  emit(commonToken(NEWLINE, newLine, last - this->getText().length() + 1,
                   last - spaces.length()));
  size_t indentation = spaces.length();

  size_t previous = indents.empty() ? 0 : indents.top();
  if (indentation > previous) {
    indents.push(indentation);
    emit(commonToken(TestParser::INDENT, spaces, last - spaces.length() + 1,
                     last));
  }
}};
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
