#include "TestBaseListener.h"
#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <iostream>

using namespace parser;
using namespace antlr4;

class IdListener : public parser::TestBaseListener {
public:
  void exitS_indent(TestParser::S_indentContext *context) {
    std::cout << "Found “" << context->getText() << "”" << std::endl;
  }
};

int main() {
  ANTLRInputStream input(u8"  hello\n  world\n");
  TestLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  TestParser parser(&tokens);
  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.nodes();
  walker.walk(&listener, tree);
  return 0;
}
