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

using antlr4::CharStream;
using antlr4::CommonToken;
using antlr4::CommonTokenFactory;
using antlr4::Token;
using antlr4::TokenFactory;
using antlr4::TokenSource;

using std::deque;
using std::pair;
using std::string;
using std::unique_ptr;

// -- Class --------------------------------------------------------------------

class YAMLLexer : public TokenSource {
  CharStream *input;
  deque<unique_ptr<CommonToken>> tokens;
  Ref<TokenFactory<CommonToken>> factory = CommonTokenFactory::DEFAULT;
  pair<TokenSource *, CharStream *> source;
  size_t line = 1;
  size_t column = 0;

  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop,
                                      string text);
  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop);

  void fetchTokens();
  void scanStart();
  void scanEnd();
  void scanPlainScalar();

public:
  static const size_t STREAM_START = 1;
  static const size_t STREAM_END = 2;
  static const size_t PLAIN_SCALAR = 3;

  YAMLLexer(CharStream *input);

  unique_ptr<Token> nextToken() override;
  size_t getLine() const override;
  size_t getCharPositionInLine() override;
  CharStream *getInputStream() override;
  std::string getSourceName() override;
  template <typename T1> void setTokenFactory(TokenFactory<T1> *factory);
  Ref<TokenFactory<CommonToken>> getTokenFactory() override;
};
