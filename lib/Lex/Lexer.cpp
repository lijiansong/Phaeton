//===--- Lexer.cpp - Phaeton Language Lexer -------------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements the Lexer and Token interfaces.
//
//===----------------------------------------------------------------------===//

#include "ph/Lex/Lexer.h"

#include "lex.yy.h"

#include <map>
#include <string>

using namespace phaeton;

Lexer::Lexer(const char *In) : Input(In) {
  yylex_init(&Scanner);
  yy_scan_string(Input, Scanner);
}

Lexer::~Lexer() { yylex_destroy(Scanner); }

int Lexer::lex() { return yylex(&Val, Scanner); }

std::map<int, const std::string> Lexer::TokenStrs = {
    {KW_VAR, "KW_VAR"},
    {KW_INPUT, "KW_INPUT"},
    {KW_OUTPUT, "KW_OUTPUT"},
    {KW_TYPE, "KW_TYPE"},
    {SYMBOL_COLON, "SYMBOL_COLON"},
    {SYMBOL_LEFT_PAREN, "SYMBOL_LEFT_PAREN"},
    {SYMBOL_RIGHT_PAREN, "SYMBOL_RIGHT_PAREN"},
    {SYMBOL_LEFT_BRACKET, "SYMBOL_LEFT_BRACKET"},
    {SYMBOL_RIGHT_BRACKET, "SYMBOL_RIGHT_BRACKET"},
    {ARITHM_ADD, "ARITHM_ADD"},
    {ARITHM_SUB, "ARITHM_SUB"},
    {ARITHM_MUL, "ARITHM_MUL"},
    {ARITHM_DIV, "ARITHM_DIV"},
    {ARITHM_DOT, "ARITHM_DOT"},
    {EQUAL, "EQUAL"},
    {INT, "INT"},
    {ID, "ID"}};
