/*
 * This lexer uses the same idea as the scanner of `libyaml` (and various other
 * YAML libs) to detect simple keys (keys with no `?` prefix).
 *
 * For a detailed explanation of the algorithm, I recommend to take a look at
 * the scanner of
 *
 * - SnakeYAML Engine:
 *   https://bitbucket.org/asomov/snakeyaml-engine
 * - or LLVM’s YAML library:
 *   https://github.com/llvm-mirror/llvm/blob/master/lib/Support/YAMLParser.cpp
 *
 * .
 */

// -- Imports ------------------------------------------------------------------

#include <antlr4-runtime.h>

#include "YAMLLexer.hpp"

using std::make_pair;

// -- Class --------------------------------------------------------------------

/**
 * @brief This constructor creates a new YAML lexer for the given input.
 *
 * @param input This character stream stores the data this lexer scans.
 */
YAMLLexer::YAMLLexer(CharStream *input) {
  this->input = input;
  this->source = make_pair(this, input);
  scanStart();
}

/**
 * @brief This method retrieves the current (not already emitted) token
 *        produced by the lexer.
 *
 * @return A token of the token stream produced by the lexer
 */
unique_ptr<Token> YAMLLexer::nextToken() {
  if (tokens.empty()) {
    fetchTokens();
  }

  unique_ptr<CommonToken> token = move(tokens.front());
  tokens.pop_front();
  return token;
}

/**
 * @brief This method retrieves the current line index.
 *
 * @return The index of the line the lexer is currently scanning
 */
size_t YAMLLexer::getLine() const { return line; }

/**
 * @brief This method returns the position in the current line.
 *
 * @return The character index in the line the lexer is scanning
 */
size_t YAMLLexer::getCharPositionInLine() { return column; }

/**
 * @brief This method returns the source the lexer is scanning.
 *
 * @return The input of the lexer
 */
CharStream *YAMLLexer::getInputStream() { return input; }

/**
 * @brief This method retrieves the name of the source the lexer is currently
 *        scanning.
 *
 * @return The name of the current input source
 */
std::string YAMLLexer::getSourceName() { return input->getSourceName(); }

/**
 * @brief This setter changes the token factory of the lexer.
 *
 * @param factory This parameter specifies the factory that the scanner
 *                should use to create tokens.
 */
template <typename T1>
void YAMLLexer::setTokenFactory(TokenFactory<T1> *factory) {
  this->factory = factory;
}

/**
 * @brief Retrieve the current token factory.
 *
 * @return The factory the scanner uses to create tokens
 */
Ref<TokenFactory<CommonToken>> YAMLLexer::getTokenFactory() { return factory; }

// ===================
// = Private Methods =
// ===================

/**
 * This function creates a new token with the specified parameters.
 *
 * @param type This parameter specifies the type of the token this function
 *             should create.
 * @param start This number specifies the start index of the returned token
 *              inside the character stream `input`.
 * @param stop This number specifies the stop index of the returned token
 *             inside the character stream `input`.
 *
 * @return A token with the specified parameters
 */
unique_ptr<CommonToken> YAMLLexer::commonToken(size_t type, size_t start,
                                               size_t stop) {
  return factory->create(source, type, "", Token::DEFAULT_CHANNEL, start, stop,
                         line, column);
}

/**
 * This function creates a new token with the specified parameters.
 *
 * @param type This parameter specifies the type of the token this function
 *             should create.
 * @param start This number specifies the start index of the returned token
 *              inside the character stream `input`.
 * @param stop This number specifies the stop index of the returned token
 *             inside the character stream `input`.
 * @param text This string specifies the text of the returned token.
 *
 * @return A token with the specified parameters
 */
unique_ptr<CommonToken> YAMLLexer::commonToken(size_t type, size_t start,
                                               size_t stop, string text) {
  return factory->create(source, type, text, Token::DEFAULT_CHANNEL, start,
                         stop, line, column);
}

/**
 * @brief This method adds new tokens to the token stream.
 */
void YAMLLexer::fetchTokens() {
  scanToNextToken();

  if (input->LA(1) == Token::EOF) {
    scanEnd();
    return;
  }

  scanPlainScalar();
}

/**
 * @brief This method consumes a character from the input stream keeping
 *        track of line and column numbers.
 */
void YAMLLexer::forward() {
  if (input->LA(1) == Token::EOF) {
    return;
  }

  column++;
  if (input->LA(1) == '\n') {
    column = 0;
    line++;
  }
  input->consume();
}

/**
 * @brief This method removes uninteresting characters from the input.
 */
void YAMLLexer::scanToNextToken() {
  while (input->LA(1) == ' ') {
    forward();
  }
  if (input->LA(1) == '\n') {
    forward();
  }
}

/**
 * @brief This method adds the token for the start of the YAML stream to
 *        `tokens`.
 */
void YAMLLexer::scanStart() {
  auto start =
      commonToken(STREAM_START, input->index(), input->index(), "START");
  tokens.push_back(move(start));
}

/**
 * @brief This method adds the end markers to the token queue.
 */
void YAMLLexer::scanEnd() {
  tokens.push_back(
      commonToken(STREAM_END, input->index(), input->index(), "END"));
  tokens.push_back(
      commonToken(Token::EOF, input->index(), input->index(), "EOF"));
}

/**
 * @brief This method scans a plain scalar and adds it to the token queue.
 */
void YAMLLexer::scanPlainScalar() {
  size_t start = input->index();

  while (input->LA(1) != ' ' && input->LA(1) != '\n' &&
         input->LA(1) != Token::EOF) {
    forward();
  }
  tokens.push_back(commonToken(PLAIN_SCALAR, start, input->index() - 1));
}
