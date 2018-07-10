parser grammar YAML;

options {
  tokenVocab=YAML;
}

yaml : STREAM_START STREAM_END EOF ;
