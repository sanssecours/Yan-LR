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

#include "YAMLParser.h"
}

@lexer::postinclude
{
  using std::deque;
  using std::make_pair;
  using std::regex;
  using std::stack;
  using std::string;
  using std::unique_ptr;

  using antlr4::CommonToken;
  using antlr4::Token;
}

@lexer::members
{
public:
  void emit(unique_ptr<Token> token) override {
    tokens.push_back(static_cast<CommonToken *>(&*token));
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

      // Add newline token after last node if it is missing
      if (tokens.back().getType() != NEWLINE &&
          tokens.back().getType() != YAMLParser::DEDENT) {
        emit(commonToken(NEWLINE, "\n"));
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
  deque<CommonToken> tokens{static_cast<CommonToken *>(&*indent(0))};
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
element : C_SEQUENCE_ENTRY S_SPACE scalar
        | C_SEQUENCE_ENTRY S_SPACE? NEWLINE child ;
scalar : (c_double_quoted | nb_double_one_line) NEWLINE ;

// [107]
nb_double_char : c_ns_esc_char | nb_json_minus_backslash_double_quote ;
// [109]
c_double_quoted : '"' nb_double_one_line '"' ;
// [111]
nb_double_one_line : nb_double_char* ;

nb_json_minus_backslash_double_quote : c_printable_7_bit_without_indicators
                                     | c_indicator_without_quotes
                                     | '\''
                                     | '\u007F'
                                     | C_PRINTABLE_16_BIT
                                     | NB_JSON
                                     ;

c_indicator_without_quotes : C_SEQUENCE_ENTRY
                           | C_MAPPING_KEY
                           | C_MAPPING_VALUE
                           | C_COLLECT_ENTRY
                           | C_SEQUENCE_START
                           | C_SEQUENCE_END
                           | C_MAPPING_START
                           | C_MAPPING_END
                           | C_COMMENT
                           | C_ANCHOR
                           | C_ALIAS
                           | C_TAG
                           | C_LITERAL
                           | C_FOLDED
                           | C_DIRECTIVE
                           | C_RESERVED
                           ;

// [22]
c_indicator : c_indicator_without_quotes
            | C_SINGLE_QUOTE
            | C_DOUBLE_QUOTE
            ;

// [35]
ns_dec_digit : '0'
             | '1'
             | '2'
             | '3'
             | '4'
             | '5'
             | '6'
             | '7'
             | '8'
             | '9'
             ;

ns_letter_lower_af : 'a'
                   | 'b'
                   | 'c'
                   | 'd'
                   | 'e'
                   | 'f'
                   ;

ns_letter_upper_af : 'A'
                   | 'B'
                   | 'C'
                   | 'D'
                   | 'E'
                   | 'F'
                   ;
// [36]
ns_hex_digit : ns_dec_digit
             | ns_letter_lower_af
             | ns_letter_upper_af
             ;

ns_letter_lower : ns_letter_lower_af
                | 'g'
                | 'h'
                | 'i'
                | 'j'
                | 'k'
                | 'l'
                | 'm'
                | 'n'
                | 'o'
                | 'p'
                | 'q'
                | 'r'
                | 's'
                | 't'
                | 'u'
                | 'v'
                | 'w'
                | 'x'
                | 'y'
                | 'z'
                ;

ns_letter_upper : ns_letter_upper_af
                | 'G'
                | 'H'
                | 'I'
                | 'J'
                | 'K'
                | 'L'
                | 'M'
                | 'N'
                | 'O'
                | 'P'
                | 'Q'
                | 'R'
                | 'S'
                | 'T'
                | 'U'
                | 'V'
                | 'W'
                | 'X'
                | 'Y'
                | 'Z'
                ;

// [37]
ns_ascii_letter : ns_letter_lower | ns_letter_upper ;

// [38]
ns_word_char : ns_dec_digit | ns_ascii_letter | '-' ;

c_printable_7_bit_without_indicators : S_TAB
                                     | S_SPACE
                                     | '$'
                                     | '('
                                     | ')'
                                     | '+'
                                     | '.'
                                     | '/'
                                     | ns_word_char
                                     | ';'
                                     | '<'
                                     | '='
                                     | '>'
                                     | '\\'
                                     | '^'
                                     | '_'
                                     | '|'
                                     | '~'
                                     ;

// [41]
c_escape : '\\' ;
// [42]
ns_esc_null : '0' ;
// [43]
ns_esc_bell : 'a' ;
// [44]
ns_esc_backspace : 'b' ;
// [45]
ns_esc_horizontal_tab : 't' | S_TAB ;
// [46]
ns_esc_line_feed : 'n' ;
// [47]
ns_esc_vertical_tab : 'v' ;
// [48]
ns_esc_form_feed : 'f' ;
// [49]
ns_esc_carriage_return : 'r' ;
// [50]
ns_esc_escape : 'e' ;
// [51]
ns_esc_space : ' ' ;
// [52]
ns_esc_double_quote : '"' ;
// [53]
ns_esc_slash : '/' ;
// [54]
ns_esc_backslash : '\\' ;
// [55]
ns_esc_next_line : 'N' ;
// [56]
ns_esc_non_breaking_space : '_' ;
// [57]
ns_esc_line_separator : 'L' ;
// [58]
ns_esc_paragraph_separator : 'P' ;

double_hex : ns_hex_digit ns_hex_digit;
quad_hex : double_hex double_hex;
octo_hex : quad_hex quad_hex;

// [59]
ns_esc_8_bit : 'x' double_hex ;
// [60]
ns_esc_16_bit : 'u' quad_hex ;
// [61]
ns_esc_32_bit : 'U' octo_hex ;

// [62]
c_ns_esc_char : c_escape
              ( ns_esc_null | ns_esc_bell | ns_esc_backspace
              | ns_esc_horizontal_tab | ns_esc_line_feed
              | ns_esc_vertical_tab | ns_esc_form_feed
              | ns_esc_carriage_return | ns_esc_escape | ns_esc_space
              | ns_esc_double_quote | ns_esc_slash | ns_esc_backslash
              | ns_esc_next_line | ns_esc_non_breaking_space
              | ns_esc_line_separator | ns_esc_paragraph_separator
              | ns_esc_8_bit | ns_esc_16_bit | ns_esc_32_bit ) ;

// -- Lexer Rules --------------------------------------------------------------

NEWLINE : ( '\r'? '\n' ) S_SPACE* {
  {
    string newLine = regex_replace(getText(), regex("[^\r\n]"), "");
    string spaces = regex_replace(getText(), regex("[\r\n]"), "");
    size_t last = getCharIndex() - 1;
    emit(commonToken(NEWLINE, newLine, last - getText().length() + 1,
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

// [4]
C_SEQUENCE_ENTRY : '-' ;
// [5]
C_MAPPING_KEY : '?' ;
// [6]
C_MAPPING_VALUE : ':' ;

// [7]
C_COLLECT_ENTRY : ',' ;
// [8]
C_SEQUENCE_START : '[' ;
// [9]
C_SEQUENCE_END : ']' ;
// [10]
C_MAPPING_START : '{' ;
// [11]
C_MAPPING_END : '}' ;

// [12]
C_COMMENT : '#' ;

// [13]
C_ANCHOR : '&' ;
// [14]
C_ALIAS : '*' ;
// [15]
C_TAG : '!' ;

// [16]
C_LITERAL : '|' ;
// [17]
C_FOLDED : '>' ;

// [18]
C_SINGLE_QUOTE : '\'' ;
// [19]
C_DOUBLE_QUOTE : '"' ;

// [20]
C_DIRECTIVE : '%' ;

// [21]
C_RESERVED : '@' | '`' ;

// [31]
S_SPACE : ' ' ;
// [32]
S_TAB : '\t' ;


C_PRINTABLE_16_BIT : '\u0085'
                   | [\u00A0-\uD7FF]
                   | [\uE000-\uFFFD]
                   | [\u{10000}-\u{10FFFF}]
                   ;

NB_JSON : [\u0020-\u{10FFFF}] ;
