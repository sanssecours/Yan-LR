#include <antlr4-runtime.h>

using antlr4::Token;

class YAMLLexer : public TokenSource {
public:
  static const size_t STREAM_BEGIN = 1;
  static const size_t STREAM_END = 2;

  YAMLLexer(CharStream *input) { this->input = input; }

  std::unique_ptr<Token> nextToken() override {
    return factory->create(make_pair(this, input), Token::EOF, "",
                           Token::DEFAULT_CHANNEL, 0, 0, 0, 0);
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
  Ref<TokenFactory<CommonToken>> factory = CommonTokenFactory::DEFAULT;
};
