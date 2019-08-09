//===---- Lexer.h - Lexer for DSL Phaeton -----------------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Lexer interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_LEXER_H
#define PHAETON_LEXER_H

#include "phaeton.tab.hh"

#include <fstream>
#include <map>
#include <string>

namespace phaeton {

class Lexer {
private:
  const char *Input;

  yyscan_t Scanner;
  YYSTYPE Val;

public:
  Lexer(const char *Input);
  ~Lexer();

  int lex();

  yyscan_t getScanner() const { return Scanner; }
  YYSTYPE getVal() const { return Val; }

  static const std::string &getTokenString(int Token) {
    return TokenStrings[Token];
  }

  static std::map<int, const std::string> TokenStrings;
};

} // end namespace phaeton

#endif // PHAETON_LEXER_H
