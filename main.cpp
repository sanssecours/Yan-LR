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

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }
  return 0;
}
