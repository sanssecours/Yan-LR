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

#include "YAMLLexer.hpp"

using std::endl;
using std::make_pair;

using antlr4::ParseCancellationException;

using spdlog::set_level;
using spdlog::set_pattern;
using spdlog::stderr_logger_mt;
using spdlog::level::trace;

// -- Class --------------------------------------------------------------------

/**
 * @brief This constructor creates a new YAML lexer for the given input.
 *
 * @param input This character stream stores the data this lexer scans.
 */
YAMLLexer::YAMLLexer(CharStream *input) {
  set_pattern("[%H:%M:%S:%e] %v ");
  set_level(trace);
  console = stderr_logger_mt("console");
  LOG("Init lexer");

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
  LOG("Retrieve next token");
  if (tokens.empty()) {
    fetchTokens();
  }
  LOG("Tokens:");
  for (unique_ptr<CommonToken> const &token : tokens) {
    LOGF("\t {}", token->toString());
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
  } else if (input->LA(1) == ':' && input->LA(2) == ' ') {
    scanValue();
  }

  scanPlainScalar();
}

/**
 * @brief This method consumes characters from the input stream keeping
 *        track of line and column numbers.
 *
 * @param characters This parameter specifies the number of characters the
 *                   the function should consume.
 */
void YAMLLexer::forward(size_t const characters = 1) {
  for (size_t charsLeft = characters; charsLeft > 0; charsLeft--) {
    if (input->LA(1) == Token::EOF) {
      LOG("Hit EOF!");
      return;
    }

    column++;
    if (input->LA(1) == '\n') {
      column = 0;
      line++;
    }
    input->consume();
  }
}

/**
 * @brief This method removes uninteresting characters from the input.
 */
void YAMLLexer::scanToNextToken() {
  LOG("Scan to next token");
  bool found = false;
  while (!found) {
    while (input->LA(1) == ' ') {
      forward();
    }
    LOG("Skipped whitespace");
    if (input->LA(1) == '\n') {
      forward();
      LOG("Skipped newline");
    } else {
      found = true;
      LOG("Found next token");
    }
  }
}

/**
 * @brief This method adds the token for the start of the YAML stream to
 *        `tokens`.
 */
void YAMLLexer::scanStart() {
  LOG("Scan start");
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
  LOG("Scan plain scalar");
  size_t start = input->index();
  // A plain scalar can start a simple key
  simpleKey = commonToken(KEY, start, start, "KEY");

  string const stop = " \n";

  while (stop.find(input->LA(1)) == string::npos &&
         input->LA(1) != Token::EOF &&
         !(input->LA(1) == ':' && input->LA(2) == ' ')) {
    forward();
  }
  tokens.push_back(commonToken(PLAIN_SCALAR, start, input->index() - 1));
}

/**
 * @brief This method scans a mapping value and adds it to the token queue.
 */
void YAMLLexer::scanValue() {
  LOG("Scan value");
  tokens.push_back(commonToken(VALUE, input->index(), input->index() + 1));
  forward(2);
  if (!simpleKey) {
    throw ParseCancellationException("Unable to locate key for value");
  }
  if (simpleKey->getStartIndex() < column) {
    tokens.push_front(
        commonToken(MAPPING_START, simpleKey->getStartIndex(), column));
  }
  tokens.push_front(move(simpleKey));
}
