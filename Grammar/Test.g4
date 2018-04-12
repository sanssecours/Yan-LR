grammar Test;

nodes : node+ EOF;
node : (level1 | level2) NL ;
level1 : s_indent[0] ID;
level2 : s_indent[2] ID;

s_indent[int spaces] :
    {$spaces != 0}? SPACE s_indent[spaces-1]
  | {$spaces == 0}?
  ;

ID : [a-zA-Z0-9]+ ;
SPACE : [ ] ;
NL : '\n' ;
