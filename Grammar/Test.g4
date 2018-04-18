grammar Test;

@header
{
#include <iostream>
}

@postinclude
{
using namespace std;
using namespace antlr4;
}

@parser::members
{
bool indent(int spaces) {
  Token *next = getCurrentToken();
  cout << "indent(" << spaces << ")" << endl;
  cout << "next: " << next->getText() << endl;
  if (next && next->getType() == SPACES) {
    cout << "Spaces: " << next->getText().length() << endl;
  }
  return spaces==0 || (next->getType() == SPACES &&
                       next->getText().length() == spaces);
}
}

nodes : node+ EOF;
node : (level1 | level2 | level3) NL ;
level1 : { indent(0) }? ID ;
level2 : { indent(2) }? SPACES ID ;
level3 : { indent(4) }? SPACES ID ;
spaces : SPACES ;

NL : '\n' ;
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
