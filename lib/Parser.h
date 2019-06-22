#ifndef __PARSER_H__
#define __PARSER_H__

#include "AST.h"
#include "Lexer.h"
#include "lang.tab.hh"

class Parser {
private:
  Lexer lexer;

  const Program *ast;

public:
  Parser(const char *input) : lexer(input) {}

  ~Parser() {}

  int parse() { return yyparse(lexer.getScanner(), ast); }

  const Program *getAST() const { return ast; }
};

#endif /* !__PARSER_H__ */
