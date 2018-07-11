#include <antlr4-runtime.h>

using antlr4::BaseErrorListener;
using antlr4::Recognizer;
using antlr4::Token;

using std::exception_ptr;
using std::string;

class ErrorListener : public BaseErrorListener {
  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const string &message,
                   exception_ptr error);
};
