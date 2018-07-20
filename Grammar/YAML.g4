parser grammar YAML;

options {
  tokenVocab=YAML;
}

yaml : STREAM_START child? STREAM_END EOF ;
child : value | map | sequence;

map : MAPPING_START pairs BLOCK_END ;
pairs : pair+ ;
pair : KEY key VALUE value;
key : scalar ;
value : scalar ;

sequence : SEQUENCE_START elements BLOCK_END ;
elements : element+ ;
element : ELEMENT value ;

scalar : PLAIN_SCALAR;
