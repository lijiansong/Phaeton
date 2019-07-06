//===--- Parser.h - Parser for DSL TensorIR ---------------------*- C++ -*-===//
//
//  This file defines the Parser interface.
//
//===----------------------------------------------------------------------===//

#ifndef __PARSER_H__
#define __PARSER_H__

#include "tir/AST/AST.h"
#include "tir/Lex/Lexer.h"

class Parser {
private:
  Lexer lexer;

  const Program *ast;

public:
  Parser(const char *input) : lexer(input) {}

  ~Parser() {}

  int parse();

  const Program *getAST() const { return ast; }
};

#endif // __PARSER_H__
