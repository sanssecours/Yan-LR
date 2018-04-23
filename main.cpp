#include "TestBaseListener.h"
#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <iostream>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::TestBaseListener {};

int main() {
  const string text =
      u8"parent1\n  child1\n  child2\n    grandchild\nparent2\n";
  cout << "——————————" << endl << text << "——————————" << endl;
  ANTLRInputStream input(text);
  TestLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  TestParser parser(&tokens);
  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.nodes();
  walker.walk(&listener, tree);
  return 0;
}
