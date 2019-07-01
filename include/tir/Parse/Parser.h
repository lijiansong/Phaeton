#ifndef __PARSER_H__
#define __PARSER_H__

#include "tir/AST/AST.h"
#include "tir/Lex/Lexer.h"

#include "lang.tab.hh"
#include "lex.yy.h"

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

#endif /* !__PARSER_H__ */
