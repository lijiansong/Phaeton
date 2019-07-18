//===---- Lexer.h - Lexer for DSL Phaeton -----------------------*- C++ -*-===//
//
//  This file defines the Lexer interface.
//
//===----------------------------------------------------------------------===//

#ifndef __LEXER_H__
#define __LEXER_H__

#include <fstream>
#include <map>
#include <string>

#include "phaeton.tab.hh"

class Lexer {
private:
  const char *Input;

  yyscan_t Scanner;
  YYSTYPE Val;

public:
  Lexer(const char *input);
  ~Lexer();

  int lex();

  yyscan_t getScanner() const { return Scanner; }
  YYSTYPE getVal() const { return Val; }

  static std::map<int, const std::string> TokenStrings;

  static const std::string &getTokenString(int token) {
    return TokenStrings[token];
  }
};

#endif // __LEXER_H__