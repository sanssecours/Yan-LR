#include "ErrorListener.hpp"
#include "YAMLBaseListener.h"
#include "YAMLLexer.h"
#include "YAMLParser.h"
#include <antlr4-runtime.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdlib.h>

using namespace parser;
using namespace antlr4;
using namespace std;

class IdListener : public parser::YAMLBaseListener {
  deque<string> path;

  virtual void exitValue(YAMLParser::ValueContext *context) override {
    string output = accumulate(path.begin() + 1, path.end(), path[0],
                               [](const string &accumulator, string value) {
                                 return accumulator + "/" + value;
                               });

    cout << output << ": " << context->getText() << endl;
  }

  virtual void enterMapping(YAMLParser::MappingContext *context) override {
    string key = context->key()->getText();
    path.push_back(key);
  }

  virtual void exitMapping(YAMLParser::MappingContext *context
                           __attribute__((unused))) override {
    path.pop_back();
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
