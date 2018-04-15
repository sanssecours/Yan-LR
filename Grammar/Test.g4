grammar Test;

@header
{
  #include <iostream>
  using namespace std;
}

@parser::members
{
  int n = -1;
  antlr4::Token *last;
}

nodes : node+ EOF;
node : (level1 | level2 | level3) newline ;
level1 : { (!last || last->getType() == SPACES) && n <= 0 }? ID ;
level2 : { last && last->getType() == SPACES && n == 2 }? ID ;
level3 : { last && last->getType() == SPACES && n == 4 }? ID ;

newline : '\n' SPACES?
        {
          if ($SPACES) {
            n = $SPACES.text.length();
            last = $SPACES;
          }
        }
        ;

ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
