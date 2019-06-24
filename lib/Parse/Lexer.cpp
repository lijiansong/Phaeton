#include <map>
#include <string>

#include "tir/Parse/Lexer.h"

using TokenStrType = std::map<int, const std::string>;

TokenStrType Lexer::TokenStrings = {
    {KW_VAR, "KW_VAR"}, {KW_TYPE, "KW_TYPE"}, {COLON, "COLON"},
    {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"},   {LBRACK, "LBRACK"},
    {RBRACK, "RBRACK"}, {STAR, "STAR"},       {DOT, "DOT"},
    {EQUAL, "EQUAL"},   {INT, "INT"},         {ID, "ID"}};
