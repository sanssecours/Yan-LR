// -- Imports ------------------------------------------------------------------

#include <iostream>

#include "ErrorListener.hpp"

using std::cerr;
using std::endl;

// -- Class --------------------------------------------------------------------

void ErrorListener::syntaxError(Recognizer *recognizer __attribute__((unused)),
                                Token *offendingSymbol __attribute__((unused)),
                                size_t line, size_t charPositionInLine,
                                const std::string &message,
                                std::exception_ptr error
                                __attribute__((unused))) {
  cerr << line << ":" << charPositionInLine << " " << message << endl;
  exit(EXIT_FAILURE);
}
