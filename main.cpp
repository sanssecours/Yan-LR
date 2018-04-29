#include "TestBaseListener.h"
#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <fstream>
#include <iostream>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::TestBaseListener {};

int main() {
  ifstream file{"Test.yaml"};
  stringstream text;
  text << file.rdbuf();
  cout << "——————————" << endl << text.str() << "——————————" << endl;
  ANTLRInputStream input(text.str());
  TestLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  TestParser parser(&tokens);
  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.nodes();
  walker.walk(&listener, tree);
  return 0;
}
