//===---- Parser.h - Parser for DSL Phaeton ---------------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Parser interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_PARSER_H
#define PHAETON_PARSER_H

#include "ph/AST/AST.h"
#include "ph/Lex/Lexer.h"

namespace phaeton {

class Parser {
private:
  Lexer Lexer;

  const Program *AST;

public:
  Parser(const char *Input) : Lexer(Input) {}

  ~Parser() {}

  int parse();

  const Program *getAST() const { return AST; }
};

} // end namespace phaeton

#endif // PHAETON_PARSER_H
