parser grammar YAML;

options {
  tokenVocab=YAML;
}

yaml : STREAM_START child? STREAM_END EOF ;
child : scalar | map;

map : MAPPING_START pair BLOCK_END ;
pair : KEY key VALUE value ;
key : scalar ;
value : scalar ;

scalar : PLAIN_SCALAR;
