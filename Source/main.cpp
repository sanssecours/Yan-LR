// -- Imports ------------------------------------------------------------------

#include <fstream>

#include <antlr4-runtime.h>
#include <kdb.hpp>

#include "YAML.h"

#include "ErrorListener.hpp"
#include "Listener.hpp"
#include "YAMLLexer.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;

using CppKey = kdb::Key;
using ckdb::keyNew;

using antlr4::ANTLRInputStream;
using antlr4::CommonTokenStream;
using ParseTree = antlr4::tree::ParseTree;
using ParseTreeWalker = antlr4::tree::ParseTreeWalker;

using antlr::YAML;

// -- Functions ----------------------------------------------------------------

void printTokens(CommonTokenStream &tokens) {
  tokens.fill();
  cout << "— Tokens ——————" << endl << endl;
  for (auto token : tokens.getTokens()) {
    cout << token->toString() << endl;
  }
  cout << endl;
}

void printTree(ParseTree *tree) {
  cout << "— Tree ——————" << endl << endl;
  cout << tree->toStringTree() << endl << endl;
}

void printOutput(KeyListener &listener) {
  cout << "— Output ————" << endl << endl;
  for (auto key : listener.keySet()) {
    cout << key.getName() << ": " << key.getString() << endl;
  }
}

void setErrorListener(YAML &parser) {
  ErrorListener errorListener{};
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);
}

// -- Main ---------------------------------------------------------------------

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
  cout << "— Input ———————" << endl << endl << text.str() << endl;
  ANTLRInputStream input(text.str());
  YAMLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  printTokens(tokens);

  YAML parser(&tokens);
  setErrorListener(parser);

  ParseTree *tree = parser.yaml();
  printTree(tree);

  ParseTreeWalker walker{};
  KeyListener listener{keyNew("user", KEY_END, "", KEY_VALUE)};
  walker.walk(&listener, tree);
  printOutput(listener);

  return EXIT_SUCCESS;
}
