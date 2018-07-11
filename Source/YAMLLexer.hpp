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

using antlr4::Token;

class YAMLLexer : public TokenSource {
public:
  static const size_t STREAM_START = 1;
  static const size_t STREAM_END = 2;
  static const size_t PLAIN_SCALAR = 3;

  YAMLLexer(CharStream *input) {
    this->input = input;
    this->source = make_pair(this, input);
    scanStart();
  }

  unique_ptr<Token> nextToken() override {
    if (tokens.empty()) {
      fetchTokens();
    }
    unique_ptr<CommonToken> token = move(tokens.front());
    tokens.pop_front();
    return token;
  }

  size_t getLine() const override { return line; }

  size_t getCharPositionInLine() override { return column; }

  CharStream *getInputStream() override { return input; }

  std::string getSourceName() override { return input->getSourceName(); }

  template <typename T1> void setTokenFactory(TokenFactory<T1> *factory) {
    this->factory = factory;
  }

  Ref<TokenFactory<CommonToken>> getTokenFactory() override { return factory; }

private:
  CharStream *input;
  deque<unique_ptr<CommonToken>> tokens;
  Ref<TokenFactory<CommonToken>> factory = CommonTokenFactory::DEFAULT;
  pair<TokenSource *, CharStream *> source;
  size_t line = 1;
  size_t column = 0;

  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop) {
    return factory->create(source, type, "", Token::DEFAULT_CHANNEL, start,
                           stop, line, column);
  }

  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop,
                                      string text) {
    return factory->create(source, type, text, Token::DEFAULT_CHANNEL, start,
                           stop, line, column);
  }

  void fetchTokens() {
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

  void scanStart() {
    auto start =
        commonToken(STREAM_START, input->index(), input->index(), "START");
    tokens.push_back(move(start));
  }

  void scanEnd() {
    tokens.push_back(
        commonToken(STREAM_END, input->index(), input->index(), "END"));
    tokens.push_back(
        commonToken(Token::EOF, input->index(), input->index(), "EOF"));
  }

  void scanPlainScalar() {
    size_t start = input->index();

    while (input->LA(1) != ' ' && input->LA(1) != '\n' &&
           input->LA(1) != Token::EOF) {
      input->consume();
    }
    tokens.push_back(commonToken(PLAIN_SCALAR, start, input->index() - 1));
  }
};
