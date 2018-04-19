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
  cout << string(spaces, '.') << "indent(" << spaces << ")" << endl;
  Token *next = getCurrentToken();
  cout << "next: “" << next->getText() << "”" << endl;
  if (next && next->getType() == SPACES) {
    cout << "Spaces: " << next->getText().length() << endl;
  }
  return spaces==0 || (_input->LT(1)->getType() == SPACES &&
                       _input->LT(1)->getText().length() == spaces);
}
}

nodes : node+ EOF;
node : (level1[0] | level2[2] | level3[4]) NL ;
level1 [int n] : { indent($n) }? ID ;
level2 [int n]: { indent(2) }? SPACES ID ;
level3 [int n] : { indent($n) }? SPACES ID ;

NL : '\n' ;
ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
