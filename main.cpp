#include "TestBaseListener.h"
#include "TestLexer.h"
#include "TestParser.h"
#include <antlr4-runtime.h>
#include <iostream>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::TestBaseListener {
public:
  void exitLevel1(TestParser::Level1Context *context) {
    cout << "Found 🙋🏼‍♀️ “" << context->ID()->getText() << "”"
         << endl;
  }

  void exitLevel2(TestParser::Level2Context *context) {
    cout << "Found 🧒🏾 “" << context->ID()->getText() << "”" << endl;
  }

  void exitLevel3(TestParser::Level3Context *context) {
    cout << "Found 👼🏻 “" << context->ID()->getText() << "”" << endl;
  }
};

int main() {
  const string text = u8"parent\n  child\n    grandchild\n";
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
