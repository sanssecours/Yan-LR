#include "TestBaseListener.h"
#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <iostream>

using namespace parser;
using namespace antlr4;

class IdListener : public parser::TestBaseListener {
public:
  void exitId(TestParser::IdContext *context) {
    std::cout << "Found “" << context->getText() << "”" << std::endl;
  }
};

int main() {
  ANTLRInputStream input(u8"hello world");
  TestLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  TestParser parser(&tokens);
  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.ids();
  walker.walk(&listener, tree);
  return 0;
}
