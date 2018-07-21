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
  while ((tokens.empty() || simpleKey.first != nullptr) &&
         input->LA(1) != Token::EOF) {
    fetchTokens();
    LOG("Tokens:");
    for (unique_ptr<CommonToken> const &token : tokens) {
      LOGF("\t {}", token->toString());
    }
  }

  unique_ptr<CommonToken> token = move(tokens.front());
  tokens.pop_front();
  tokensEmitted++;
  LOGF("Emit token {}", token->toString());
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
 * @param tokenFactory This parameter specifies the factory that the scanner
 *                     should use to create tokens.
 */
template <typename T1>
void YAMLLexer::setTokenFactory(TokenFactory<T1> *tokenFactory) {
  factory = tokenFactory;
}

/**
 * @brief Retrieve the current token factory.
 *
 * @return The factory the scanner uses to create tokens
 */
Ref<TokenFactory<CommonToken>> YAMLLexer::getTokenFactory() { return factory; }

// ===========
// = Private =
// ===========

/**
 * @brief This function creates a new token with the specified parameters.
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
 * @brief This function creates a new token with the specified parameters.
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
 * @brief This function adds an indentation value if the given value is smaller
 *        than the current indentation.
 *
 * @param lineIndex This parameter specifies the indentation value that this
 *                  function compares to the current indentation.
 *
 * @retval true If the function added an indentation value
 *         false Otherwise
 */
bool YAMLLexer::addIndentation(size_t const lineIndex) {
  if (static_cast<long long>(lineIndex) > indents.top()) {
    LOGF("Add indentation {}", column);
    indents.push(lineIndex);
    return true;
  }
  return false;
}

/**
 * @brief This method adds new tokens to the token stream.
 */
void YAMLLexer::fetchTokens() {
  scanToNextToken();

  addBlockEnd(column);

  if (input->LA(1) == Token::EOF) {
    scanEnd();
    return;
  } else if (isValue()) {
    scanValue();
    return;
  } else if (isElement()) {
    scanElement();
    return;
  } else if (input->LA(1) == '"') {
    scanDoubleQuotedScalar();
    return;
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
 * @brief This method checks if the input at the specified offset starts a key
 *        value token.
 *
 * @param offset This parameter specifies an offset to the current position,
 *               where this function will look for a key value token.
 *
 * @retval true If the input matches a key value token
 *         false Otherwise
 */
bool YAMLLexer::isValue(size_t const offset) {
  return (input->LA(offset) == ':') &&
         (input->LA(offset + 1) == '\n' || input->LA(offset + 1) == ' ');
}

/**
 * @brief This method checks if the current input starts a list element.
 *
 * @retval true If the input matches a list element token
 *         false Otherwise
 */
bool YAMLLexer::isElement() {
  return (input->LA(1) == '-') && (input->LA(2) == '\n' || input->LA(2) == ' ');
}

/**
 * @brief This method saves a token for a simple key candidate located at the
 *        current input position.
 */
void YAMLLexer::addSimpleKeycCandidate() {
  size_t position = tokens.size() + tokensEmitted;
  size_t index = input->index();
  simpleKey = make_pair(commonToken(KEY, index, index, "KEY"), position);
}

/**
 * @brief This method adds block closing tokens to the token queue, if the
 *        indentation decreased.
 *
 * @param lineIndex This parameter specifies the column (indentation in number
 *                  of spaces) for which this method should add block end
 *                  tokens.
 */
void YAMLLexer::addBlockEnd(long long const lineIndex) {
  while (lineIndex < indents.top()) {
    LOG("Add block end");
    size_t index = input->index();
    tokens.push_back(commonToken(BLOCK_END, index, index, "BLOCK END"));
    indents.pop();
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
  addBlockEnd(-1);
  tokens.push_back(
      commonToken(STREAM_END, input->index(), input->index(), "END"));
  tokens.push_back(
      commonToken(Token::EOF, input->index(), input->index(), "EOF"));
}

/**
 * @brief This method scans a double quoted scalar and adds it to the token
 *        queue.
 */
void YAMLLexer::scanDoubleQuotedScalar() {
  LOG("Scan double quoted scalar");
  size_t start = input->index();

  // A double quoted scalar can start a simple key
  addSimpleKeycCandidate();

  forward(); // Include initial double quote
  while (input->LA(1) != '"') {
    forward();
  }
  forward(); // Include closing double quote
  tokens.push_back(
      commonToken(DOUBLE_QUOTED_SCALAR, start, input->index() - 1));
}

/**
 * @brief This method scans a plain scalar and adds it to the token queue.
 */
void YAMLLexer::scanPlainScalar() {
  LOG("Scan plain scalar");
  size_t start = input->index();
  // A plain scalar can start a simple key
  addSimpleKeycCandidate();

  string const stop = " \n";

  while (stop.find(input->LA(1)) == string::npos &&
         input->LA(1) != Token::EOF && !isValue()) {
    forward();
  }
  tokens.push_back(commonToken(PLAIN_SCALAR, start, input->index() - 1));
}

/**
 * @brief This method scans a mapping value token and adds it to the token
 *        queue.
 */
void YAMLLexer::scanValue() {
  LOG("Scan value");
  tokens.push_back(commonToken(VALUE, input->index(), input->index() + 1));
  forward(2);
  if (simpleKey.first == nullptr) {
    throw ParseCancellationException("Unable to locate key for value");
  }
  size_t start = simpleKey.first->getCharPositionInLine();
  tokens.insert(tokens.begin() + simpleKey.second - tokensEmitted,
                move(simpleKey.first));
  if (addIndentation(start)) {
    tokens.push_front(
        commonToken(MAPPING_START, start, column, "MAPPING START"));
  }
}

/**
 * @brief This method scans a list element token and adds it to the token
 *        queue.
 */
void YAMLLexer::scanElement() {
  LOG("Scan element");
  if (addIndentation(column)) {
    tokens.push_back(
        commonToken(SEQUENCE_START, input->index(), column, "SEQUENCE START"));
  }
  tokens.push_back(commonToken(ELEMENT, input->index(), input->index() + 1));
  forward(2);
}
