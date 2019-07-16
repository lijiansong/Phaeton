//===---- Parser.h - Parser for DSL Phaeton ---------------------*- C++ -*-===//
//
//  This file defines the Parser interface.
//
//===----------------------------------------------------------------------===//

#ifndef __PARSER_H__
#define __PARSER_H__

#include "ph/AST/AST.h"
#include "ph/Lex/Lexer.h"

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
