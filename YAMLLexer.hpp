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
    unique_ptr<CommonToken> token = move(tokens.back());
    tokens.pop_back();
    return token;
  }

  size_t getLine() const override { return 0; }

  size_t getCharPositionInLine() override { return 0; }

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
      input->consume();
    }
    auto eof = commonToken(Token::EOF, input->index(), input->index(), "EOF");
    tokens.push_back(move(eof));
  }

  void scanStart() {
    auto start = commonToken(STREAM_START, 0, 0, "START");
    tokens.push_back(move(start));
  }
};
