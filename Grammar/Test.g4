grammar Test;

nodes : node+ EOF;
node : (level1 | level2 | level3) NL ;
level1 : ID;
level2 :
  { _input->LA(1) == SPACES && _input->LT(1)->getText().length() == 2 }?
  SPACES ID
  ;
level3 :
  { _input->LA(1) == SPACES && _input->LT(1)->getText().length() == 4 }?
  SPACES ID;

ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
NL : '\n' ;
