// -- Imports ------------------------------------------------------------------

#include <iostream>

#include "ErrorListener.hpp"

using std::cerr;
using std::endl;

// -- Class --------------------------------------------------------------------

/**
 * @brief This method will be called if the parsing process fails.
 *
 * @param recognizer This parameter stores the current recognizer used to
 *                   parse the input.
 * @param offendingSymbol This token caused the failure of the parsing
 *                        process.
 * @param line This number specifies the line where the parsing process
 *             failed.
 * @param charPositionInLine This number specifies the character position in
 *                           `line`, where the parsing process failed.
 * @param message This text describes the parsing failure.
 * @param error This parameter stores the exception caused by the parsing
 *              failure.
 */
void ErrorListener::syntaxError(Recognizer *recognizer __attribute__((unused)),
                                Token *offendingSymbol __attribute__((unused)),
                                size_t line, size_t charPositionInLine,
                                const std::string &message,
                                std::exception_ptr error
                                __attribute__((unused))) {
  cerr << line << ":" << charPositionInLine << " " << message << endl;
  exit(EXIT_FAILURE);
}
