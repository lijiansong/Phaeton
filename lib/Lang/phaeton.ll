/*===--- pheaton.ll - Recognize tokens for Phaeton ------------------------===*\
|*                                                                            *|
|*                     The Phaeton Compiler Infrastructure                    *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|*  This file defines the tokens for DSL Phaeton.                             *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

%option noyywrap
%option pointer
%option reentrant bison-bridge

%{
  #include <cstdlib>
  #include "phaeton.tab.hh"

  #if __cplusplus > 199711L
  #define register // 'register' keyword is deprecated in C++11.
  #endif  // #if __cplusplus > 199711L

%}

KW_VAR    "var"
KW_INPUT  "in"
KW_OUTPUT "out"
KW_TYPE   "type"
COLON     ":"
LPAREN    "("
RPAREN    ")"
LBRACK    "["
RBRACK    "]"
DOT       "."
ADD       "+"
SUB       "-"
MUL       "*"
DIV       "/"
PRODUCT   "#"
EQUAL     "="
ADD_EQUAL "+="
SUB_EQUAL "-="
MUL_EQUAL "*="
DIV_EQUAL "/="
COMMA     ","
INT       [0-9]+
ID        [_a-zA-Z][_a-zA-Z0-9]*

/* data types */
INT8      "i8"
INT16     "i16"
INT32     "i32"
FLOAT8    "f8"
FLOAT16   "f16"
FLOAT32   "f32"
FLOAT64   "f64"

/* TODO: brach control keywords*/

%%


{KW_VAR}    { return KW_VAR; }
{KW_INPUT}  { return KW_INPUT; }
{KW_OUTPUT} { return KW_OUTPUT; }
{KW_TYPE}   { return KW_TYPE; }
{COLON}     { return COLON; }
{LPAREN}    { return LPAREN; }
{RPAREN}    { return RPAREN; }
{LBRACK}    { return LBRACK; }
{RBRACK}    { return RBRACK; }
{DOT}       { return DOT; }
{ADD}       { return ADD; }
{SUB}       { return SUB; }
{MUL}       { return MUL; }
{DIV}       { return DIV; }
{PRODUCT}   { return PRODUCT; }
{EQUAL}     { return EQUAL; }
{INT}       { 
              yylval->integer_literal = atoi(yytext);
              return INT;
            }
{ID}        {
              yylval->string_literal = yytext;
              return ID;
            }
<<EOF>>     { return EOF; }
[ \t\n]+    /* ignore whitespace */

"//".*             ; /* for double slash comments*/

%%
