#include <antlr4-runtime.h>
#include <iostream>

using namespace antlr4;
using namespace std;

class ErrorListener : public BaseErrorListener {

private:
  virtual void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                           size_t line, size_t charPositionInLine,
                           const std::string &message,
                           std::exception_ptr error) override {
    cerr << line << ":" << charPositionInLine << " " << message << endl;
    exit(EXIT_FAILURE);
  }
};
