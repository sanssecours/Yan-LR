#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <iostream>

using namespace parser;
using namespace antlr4;

int main() {
  ANTLRInputStream input(u8"hello world");
  TestLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  TestParser parser(&tokens);

  tree::ParseTree *tree = parser.ids();
  std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
  return 0;
}
