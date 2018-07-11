// -- Imports ------------------------------------------------------------------

#include <antlr4-runtime.h>

using antlr4::BaseErrorListener;
using antlr4::Recognizer;
using antlr4::Token;

using std::exception_ptr;
using std::string;

// -- Class --------------------------------------------------------------------

/**
 * @brief This class specifies methods to alter error messages.
 */
class ErrorListener : public BaseErrorListener {

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
  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const string &message,
                   exception_ptr error);
};
