/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED
# define YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 16 "/Users/lijiansong/work-space/compile/git/Phaeton/lib/Lang/phaeton.yy"

  #include "ph/AST/AST.h"

  typedef void* yyscan_t;

#line 54 "/Users/lijiansong/work-space/compile/git/Phaeton/build/lib/Lang/phaeton.tab.hh"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    KW_VAR = 258,
    KW_INPUT = 259,
    KW_OUTPUT = 260,
    KW_TYPE = 261,
    COLON = 262,
    LPAREN = 263,
    RPAREN = 264,
    LBRACK = 265,
    RBRACK = 266,
    DOT = 267,
    ADD = 268,
    SUB = 269,
    MUL = 270,
    DIV = 271,
    PRODUCT = 272,
    EQUAL = 273,
    INT = 274,
    ID = 275
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 34 "/Users/lijiansong/work-space/compile/git/Phaeton/lib/Lang/phaeton.yy"

  const Program *program;
  
  DeclList *decls;
  StmtList *stmts;
  ExprList *exprs;
 
  const Decl *decl;

  const Stmt *stmt;

  const Expr *expr;

  const Identifier *identifier;
  const Integer *integer;
  const BrackExpr *brack;
  const ParenExpr *paren;

  const char *string_literal;
  int integer_literal;

  int in_out_spec;

#line 110 "/Users/lijiansong/work-space/compile/git/Phaeton/build/lib/Lang/phaeton.tab.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (yyscan_t yyscanner, const Program *&root);

#endif /* !YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED  */
