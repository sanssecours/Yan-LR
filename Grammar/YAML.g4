// Some parts of this grammar were ported from the
// [ANTLR Python 3 parser](https://github.com/bkiers/python3-parser)
grammar YAML;

tokens { INDENT, DEDENT }

@lexer::header
{
#include <iostream>
#include <memory>
#include <regex>
#include <stack>

#include "Token.h"
#include "YAMLParser.h"
}

@lexer::postinclude
{
  using namespace std;
  using namespace antlr4;
}

@lexer::members
{
public:
  void emit(unique_ptr<Token> token) override {
    tokens.push_back(dynamic_cast<CommonToken *>(&*token));
    Lexer::setToken(move(token));
  }

  unique_ptr<Token> nextToken() override {
    Lexer::nextToken();

    if (_input->LA(1) == EOF && !indents.empty()) {
      // Remove trailing EOF tokens
      // These tokens are emitted by nextToken if there is no
      // input left.
      while (!tokens.empty() && tokens.back().getType() == EOF) {
        tokens.pop_back();
      }

      if (tokens.back().getType() != YAMLParser::NEWLINE) {
        emit(commonToken(YAMLParser::NEWLINE, "\n"));
      }

      // Emit missing DEDENT tokens
      while (!indents.empty()) {
        indents.pop();
        emit(dedent(lastLine + 1));
      }

      emit(commonToken(EOF, "EOF", getCharIndex(), getCharIndex()));
    }

    unique_ptr<Token> next(new CommonToken(tokens.front()));
    tokens.pop_front();
    if (next->getChannel() == Token::DEFAULT_CHANNEL) {
      lastLine = next->getLine();
    }
    return move(next);
  }

private:
  stack<int> indents{deque<int>{0}};
  deque<CommonToken> tokens{dynamic_cast<CommonToken *>(&*indent(0))};
  int lastLine = 0;

  unique_ptr<CommonToken> commonToken(size_t type, string text) {
    size_t stop = getCharIndex() - 1;
    size_t start = text.length() <= 0 ? stop : stop - text.length() + 1;
    return move(commonToken(type, text, start, stop));
  }

  unique_ptr<CommonToken> commonToken(size_t type, string text, size_t start,
                                      size_t stop) {
    unique_ptr<CommonToken> token(new CommonToken(
        make_pair(this, _input), type, DEFAULT_TOKEN_CHANNEL, start, stop));
    return move(token);
  }

  unique_ptr<CommonToken> dedent(size_t lineNumber) {
    unique_ptr<CommonToken> token{new CommonToken{YAMLParser::DEDENT}};
    token->setLine(lineNumber);
    token->setCharPositionInLine(0);
    return move(token);
  }

  unique_ptr<CommonToken> indent(size_t lineNumber) {
    unique_ptr<CommonToken> token{new CommonToken{YAMLParser::INDENT}};
    token->setLine(lineNumber);
    token->setCharPositionInLine(0);
    return move(token);
  }
}

// -- Parser Rules -------------------------------------------------------------

yaml : child EOF ;
child : INDENT (scalar | sequence) DEDENT ;
sequence : element+ ;
element : '-' SPACE scalar
        | '-' SPACE? NEWLINE child ;
scalar : ID NEWLINE ;

// -- Lexer Rules --------------------------------------------------------------

NEWLINE : ( '\r'? '\n' ) SPACES? {
  {
    string newLine = regex_replace(this->getText(), regex("[^\r\n]"), "");
    string spaces = regex_replace(this->getText(), regex("[\r\n]"), "");
    size_t last = getCharIndex() - 1;
    emit(commonToken(NEWLINE, newLine, last - this->getText().length() + 1,
                     last - spaces.length()));
    size_t indentation = spaces.length();

    size_t previous = indents.empty() ? 0 : indents.top();
    if (indentation > previous) {
      indents.push(indentation);
      emit(commonToken(YAMLParser::INDENT, spaces, last - spaces.length() + 1,
                       last));
    } else if (indentation < previous) {
      while (!indents.empty() && indents.top() > indentation) {
        indents.pop();
        // We use the last index + 1, since the `NEWLINE` in this rule started
        // a new line
        emit(dedent(lastLine + 1));
      }
    } else {
      skip();
    }
  }
};
ID : [a-zA-Z0-9]+ ;
SPACE : ' ' ;
SPACES : SPACE+ ;

/* ===================== */
/* = 5.1 Character Set = */
/* ===================== */

fragment TAB : '\t' ;
fragment LF : '\n' ;
fragment CR : '\r' ;

/* ============================== */
/* = 5.5 White Space Characters = */
/* ============================== */

// [31]
fragment S_SPACE : ' ' ;
// [32]
fragment S_TAB : TAB ;

/* ================================ */
/* = 5.6 Miscellaneous Characters = */
/* ================================ */

// [35]
fragment NS_DEC_DIGIT : [0-9] ;
// [36]
fragment NS_HEX_DIGIT : NS_DEC_DIGIT | [A-F] | [a-f] ;

/* ========================== */
/* = 5.7 Escaped Characters = */
/* ========================== */

// [41]
fragment C_ESCAPE : '\\' ;
// [42]
fragment NS_ESC_NULL : '0' ;
// [43]
fragment NS_ESC_BELL : 'a' ;
// [44]
fragment NS_ESC_BACKSPACE : 'b' ;
// [45]
fragment NS_ESC_HORIZONTAL_TAB : 't' | TAB ;
// [46]
fragment NS_ESC_LINE_FEED : 'n' ;
// [47]
fragment NS_ESC_VERTICAL_TAB : 'v' ;
// [48]
fragment NS_ESC_FORM_FEED : 'f' ;
// [49]
fragment NS_ESC_CARRIAGE_RETURN : 'r' ;
// [50]
fragment NS_ESC_ESCAPE : 'e' ;
// [51]
fragment NS_ESC_SPACE : ' ' ;
// [52]
fragment NS_ESC_DOUBLE_QUOTE : '"' ;
// [53]
fragment NS_ESC_SLASH : '/' ;
// [54]
fragment NS_ESC_BACKSLASH : '\\' ;
// [55]
fragment NS_ESC_NEXT_LINE : 'N' ;
// [56]
fragment NS_ESC_NON_BREAKING_SPACE : '_' ;
// [57]
fragment NS_ESC_LINE_SEPARATOR : 'L' ;
// [58]
fragment NS_ESC_PARAGRAPH_SEPARATOR : 'P' ;

fragment DOUBLE_HEX : NS_HEX_DIGIT NS_HEX_DIGIT;
fragment QUAD_HEX : DOUBLE_HEX DOUBLE_HEX;
fragment OCTO_HEX : QUAD_HEX QUAD_HEX;

// [59]
fragment NS_ESC_8_BIT : 'x' DOUBLE_HEX ;
// [60]
fragment NS_ESC_16_BIT : 'u' QUAD_HEX ;
// [61]
fragment NS_ESC_32_BIT : 'U' OCTO_HEX ;

// [62]
C_NS_ESC_CHAR : C_ESCAPE
              ( NS_ESC_NULL | NS_ESC_BELL | NS_ESC_BACKSPACE
              | NS_ESC_HORIZONTAL_TAB | NS_ESC_LINE_FEED
              | NS_ESC_VERTICAL_TAB | NS_ESC_FORM_FEED
              | NS_ESC_CARRIAGE_RETURN | NS_ESC_ESCAPE | NS_ESC_SPACE
              | NS_ESC_DOUBLE_QUOTE | NS_ESC_SLASH | NS_ESC_BACKSLASH
              | NS_ESC_NEXT_LINE | NS_ESC_NON_BREAKING_SPACE
              | NS_ESC_LINE_SEPARATOR | NS_ESC_PARAGRAPH_SEPARATOR
              | NS_ESC_8_BIT | NS_ESC_16_BIT | NS_ESC_32_BIT ) ;
