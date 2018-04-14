grammar Test;

nodes : node+ EOF;
node : (level1 | level2) NL ;
level1 : ID;
level2 : s_indent[2] ID;

s_indent[int spaces] :
  SPACES {
    if ($text.length() != spaces) {
      std::cout << "Indent of " << $text.length() <<
                   " does not match expected number of spaces: " <<
                   spaces << std::endl;
      // throw RecognitionException(this, _input, _ctx);
    }
  }
  ;

ID : [a-zA-Z0-9]+ ;
SPACES : [ ]+ ;
NL : '\n' ;
