/* TODO: refact with ANTLR or self-writen lexer and parser */

/* only process single input files: */
%option noyywrap

/* make 'yytext' a character pointer: */
%option pointer

/* generate a re-entrant scanner: */
%option reentrant bison-bridge

/* generate a C header file: */
%option header-file="lex.yy.h"

%{

  #include <stdio.h>
  #include <stdlib.h>
  #include <assert.h>

  #include "AST.h"

  #include "lang.tab.hh"

  const char *get_token_string(int tok) {
    switch(tok) {
    case KW_VAR: return "KW_VAR";
    case KW_TYPE: return "KW_TYPE";
    case COLON: return "COLON";
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case LBRACK: return "LBRACK";
    case RBRACK: return "RBRACK";
    case STAR: return "STAR";
    case DOT: return "DOT";
    case EQUAL: return "EQUAL";
    case INT: return "INT";
    case ID: return "ID";
    default:
      assert(0 && "invalid token");
      return NULL;
    }
  }
%}

KW_VAR    "var"
KW_TYPE   "type"
COLON     ":"
LPAREN    "("
RPAREN    ")"
LBRACK    "["
RBRACK    "]"
STAR      "*"
EQUAL     "="
DOT       "."
INT       [0-9]+
ID        [_a-zA-Z][_a-zA-Z0-9]*

%%

{KW_VAR}  { return KW_VAR; }
{KW_TYPE} { return KW_TYPE; }
{COLON}   { return COLON; }
{LPAREN}  { return LPAREN; }
{RPAREN}  { return RPAREN; }
{LBRACK}  { return LBRACK; }
{RBRACK}  { return RBRACK; }
{STAR}    { return STAR; }
{DOT}     { return DOT; }
{EQUAL}   { return EQUAL; }
{INT}     { 
            yylval->integer_literal = atoi(yytext);
            return INT;
          }
{ID}      {
            yylval->string_literal = yytext;
            return ID;
          }
<<EOF>>   { return EOF; }
[ \t\n]+  /* ignore whitespace */ 

%%

#ifdef _LEXER_DEBUG_

int main(int argc, char* argv[]) {
  FILE *in;
  int tok;
  yyscan_t scanner;
  YYSTYPE lval;

  if (argc != 2) {
    return 1;
  } else {
    in = fopen(argv[1], "r");
    if (!in)
      return 2;
  }

  if (yylex_init(&scanner)) {
    return 3;
  }

  yyset_in(in, scanner);
  while(1) {
    tok = yylex(&lval, scanner);
    if (tok == EOF)
      break;

    printf("%s ", get_token_string(tok));
  }
  printf("\n");

  yylex_destroy(scanner);

  fclose(in);

  return 0;
}

#endif /* _LEXER_DEBUG_ */
