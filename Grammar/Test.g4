// Some parts of this grammar were ported from the
// [ANTLR Python 3 parser](https://github.com/bkiers/python3-parser)
grammar Test;

@header
{
#include <iostream>
#include <stack>
}

@postinclude
{
using namespace std;
using namespace antlr4;
}

@lexer::members
{
private:
  stack<size_t> indents;
}

nodes : node+ EOF ;
node : (ID | NEWLINE | SPACES) ;

NEWLINE : '\n' SPACES? {

};
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
