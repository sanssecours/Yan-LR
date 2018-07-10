#include <antlr4-runtime.h>

using antlr4::Token;

class YAMLLexer : public TokenSource {
public:
  static const size_t STREAM_START = 1;
  static const size_t STREAM_END = 2;

  YAMLLexer(CharStream *input) {
    this->input = input;
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
  size_t line = 1;
  size_t column = 0;

  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop) {
    return unique_ptr<CommonToken>{new CommonToken{
        make_pair(this, input), type, Token::DEFAULT_CHANNEL, start, stop}};
  }

  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop,
                                      string text) {
    auto token = commonToken(type, start, stop);
    token->setText(text);
    return token;
  }

  void fetchTokens() {
    while (input->LA(1) != Token::EOF) {
      column++;
      if (input->LA(1) == '\n') {
        line++;
        column = 0;
      }
      input->consume();
    }
    auto eof = commonToken(Token::EOF, input->index(), input->index(), "EOF");
    tokens.push_back(move(eof));
  }

  void scanStart() {
    auto start =
        commonToken(STREAM_START, input->index(), input->index(), "START");
    tokens.push_back(move(start));
  }
};
