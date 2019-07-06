%option noyywrap
%option pointer
%option reentrant bison-bridge

%{
  #include "tir.tab.hh"
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
INT       [0-9]+
ID        [_a-zA-Z][_a-zA-Z0-9]*

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

%%
