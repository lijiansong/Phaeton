#include <map>
#include <string>

#include "tir/Lex/Lexer.h"
#include "lex.yy.h"

Lexer::Lexer(const char *input) : Input(input) {
  yylex_init(&Scanner);
  yy_scan_string(Input, Scanner);
}

Lexer::~Lexer() { yylex_destroy(Scanner); }

int Lexer::lex() { return yylex(&Val, Scanner); }

std::map<int, const std::string> Lexer::TokenStrings = {
    {KW_VAR, "KW_VAR"},
    {KW_INPUT, "KW_INPUT"},
    {KW_OUTPUT, "KW_OUTPUT"},
    {KW_TYPE, "KW_TYPE"},
    {COLON, "COLON"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {LBRACK, "LBRACK"},
    {RBRACK, "RBRACK"},
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MUL, "MUL"},
    {DIV, "DIV"},
    {DOT, "DOT"},
    {EQUAL, "EQUAL"},
    {INT, "INT"},
    {ID, "ID"}};
