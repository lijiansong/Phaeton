
#include "ph/Parse/Parser.h"

int Parser::parse() { return yyparse(lexer.getScanner(), ast); }
