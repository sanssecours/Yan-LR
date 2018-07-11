parser grammar YAML;

options {
  tokenVocab=YAML;
}

yaml : STREAM_START child? STREAM_END EOF ;
child : value ;

value : PLAIN_SCALAR ;
