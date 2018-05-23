#include "ErrorListener.hpp"
#include "YAMLBaseListener.h"
#include "YAMLLexer.h"
#include "YAMLParser.h"
#include <antlr4-runtime.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::YAMLBaseListener {
  virtual void exitScalar(YAMLParser::ScalarContext *context) override {
    cout << "Matched scalar “" << context->getText() << "”" << endl;
  }
  virtual void
  exitC_double_quoted(YAMLParser::C_double_quotedContext *context) override {
    cout << "Matched double quoted scalar “"
         << context->nb_double_one_line()->getText() << "”" << endl;
  }
  virtual void exitNs_plain_one_line(
      YAMLParser::Ns_plain_one_lineContext *context) override {
    cout << "Matched plain scalar “" << context->getText() << "”" << endl;
  }
};

int main(int argc, char const *argv[]) {

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return EXIT_FAILURE;
  }

  ifstream file{argv[1]};
  if (!file.is_open()) {
    cerr << "Unable to open file “" << argv[1] << "”" << endl;
    return EXIT_FAILURE;
  }

  stringstream text;
  text << file.rdbuf();
  cout << "——————————" << endl << text.str() << "——————————" << endl;
  ANTLRInputStream input(text.str());
  YAMLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    cout << token->toString() << endl;
  }

  YAMLParser parser(&tokens);
  ErrorListener errorListener{};
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);

  tree::ParseTreeWalker walker{};
  IdListener listener{};

  antlr4::tree::ParseTree *tree = parser.yaml();
  cout << "——————————" << endl;
  cout << tree->toStringTree() << endl;
  cout << "——————————" << endl;
  walker.walk(&listener, tree);
  return EXIT_SUCCESS;
}
