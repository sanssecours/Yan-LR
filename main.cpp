#include "YAMLBaseListener.h"
#include "YAMLLexer.h"
#include "YAMLParser.h"
#include <antlr4-runtime.h>
#include <fstream>
#include <iostream>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::YAMLBaseListener {
  virtual void exitScalar(YAMLParser::ScalarContext *context) override {
    cout << "Matched scalar “" << context->getText() << "”" << endl;
  }
};

int main() {
  ifstream file{"Test.yaml"};
  stringstream text;
  text << file.rdbuf();
  cout << "——————————" << endl << text.str() << "——————————" << endl;
  ANTLRInputStream input(text.str());
  YAMLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  YAMLParser parser(&tokens);
  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.yaml();
  walker.walk(&listener, tree);
  return 0;
}
