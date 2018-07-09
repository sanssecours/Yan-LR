// This lexer uses the same idea as the scanner of `libyaml` (and various other
// YAML libs) to detect simple keys (keys with no `?` prefix).
//
// For a detailed explanation of the algorithm, I recommend to take a look at
// the scanner of
//
// - SnakeYAML Engine:
//   https://bitbucket.org/asomov/snakeyaml-engine
// - or LLVM’s YAML library:
//   https://github.com/llvm-mirror/llvm/blob/master/lib/Support/YAMLParser.cpp
//
// .
parser grammar YAML;

options {
  tokenVocab=YAML;
}

yaml : EOF ;
