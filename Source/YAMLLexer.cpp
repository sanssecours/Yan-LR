// This lexer uses the same idea as the scanner of `libyaml` (and various other
// YAML libs) to detect simple keys (keys with no `?` prefix).
//
// For a detailed explanation of the algorithm, I recommend to take a look at
// the scanner of
//
// - SnakeYAML Engine:
//   https://bitbucket.org/asomov/snakeyaml-engine
// - or LLVM’s YAML library:
//   https://github.com/llvm-mirror/llvm/blob/master/lib/Support/YAMLParser.cpp
//
// .

#include <antlr4-runtime.h>

#include "YAMLLexer.hpp"

using std::make_pair;

YAMLLexer::YAMLLexer(CharStream *input) {
  this->input = input;
  this->source = make_pair(this, input);
  scanStart();
}

unique_ptr<Token> YAMLLexer::nextToken() {
  if (tokens.empty()) {
    fetchTokens();
  }
  unique_ptr<CommonToken> token = move(tokens.front());
  tokens.pop_front();
  return token;
}

size_t YAMLLexer::getLine() const { return line; }

size_t YAMLLexer::getCharPositionInLine() { return column; }

CharStream *YAMLLexer::getInputStream() { return input; }

std::string YAMLLexer::getSourceName() { return input->getSourceName(); }

template <typename T1>
void YAMLLexer::setTokenFactory(TokenFactory<T1> *factory) {
  this->factory = factory;
}

Ref<TokenFactory<CommonToken>> YAMLLexer::getTokenFactory() { return factory; }

unique_ptr<CommonToken> YAMLLexer::commonToken(size_t type, size_t start,
                                               size_t stop) {
  return factory->create(source, type, "", Token::DEFAULT_CHANNEL, start, stop,
                         line, column);
}

unique_ptr<CommonToken> YAMLLexer::commonToken(size_t type, size_t start,
                                               size_t stop, string text) {
  return factory->create(source, type, text, Token::DEFAULT_CHANNEL, start,
                         stop, line, column);
}

void YAMLLexer::fetchTokens() {
  if (input->LA(1) == ' ' || input->LA(1) == '\n') {
    while (input->LA(1) != Token::EOF) {
      column++;
      if (input->LA(1) == '\n') {
        line++;
        column = 0;
      }
      input->consume();
    }
    scanEnd();
    return;
  }

  scanPlainScalar();
}

void YAMLLexer::scanStart() {
  auto start =
      commonToken(STREAM_START, input->index(), input->index(), "START");
  tokens.push_back(move(start));
}

void YAMLLexer::scanEnd() {
  tokens.push_back(
      commonToken(STREAM_END, input->index(), input->index(), "END"));
  tokens.push_back(
      commonToken(Token::EOF, input->index(), input->index(), "EOF"));
}

void YAMLLexer::scanPlainScalar() {
  size_t start = input->index();

  while (input->LA(1) != ' ' && input->LA(1) != '\n' &&
         input->LA(1) != Token::EOF) {
    input->consume();
  }
  tokens.push_back(commonToken(PLAIN_SCALAR, start, input->index() - 1));
}
