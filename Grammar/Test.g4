grammar Test;

nodes : node+ ;
node : s_indent[2] ID NL ;

s_indent[int n]
locals [int indent = 0;]
  : ( {$indent<$n}? SPACE {$indent++;} )*
  ;

ID : [a-zA-Z]+ ;
SPACE : [ ] ;
NL : '\n' ;
