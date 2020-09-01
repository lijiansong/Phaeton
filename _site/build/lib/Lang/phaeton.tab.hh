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
#line 16 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

  #include "ph/AST/AST.h"

  typedef void* yyscan_t;

#line 54 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.hh"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT2 = 258,
    INT8 = 259,
    INT16 = 260,
    INT32 = 261,
    INT64 = 262,
    FLOAT8 = 263,
    FLOAT16 = 264,
    FLOAT32 = 265,
    FLOAT64 = 266,
    KW_VAR = 267,
    KW_INPUT = 268,
    KW_OUTPUT = 269,
    KW_TYPE = 270,
    SYMBOL_COLON = 271,
    SYMBOL_LEFT_PAREN = 272,
    SYMBOL_RIGHT_PAREN = 273,
    SYMBOL_LEFT_BRACKET = 274,
    SYMBOL_RIGHT_BRACKET = 275,
    ARITHM_DOT = 276,
    ARITHM_ADD = 277,
    ARITHM_SUB = 278,
    ARITHM_MUL = 279,
    ARITHM_DIV = 280,
    ARITHM_PRODUCT = 281,
    EQUAL = 282,
    KW_ELEM = 283,
    KW_FIRST = 284,
    KW_LAST = 285,
    ARITHM_CARET = 286,
    INT = 287,
    ID = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 33 "/Users/lijiansong/work-space/compile/git/phaeton/lib/Lang/phaeton.yy"

  const phaeton::Program *program;
  
  phaeton::DeclList *Decls;
  phaeton::StmtList *Stmts;
  phaeton::ExprList *Exprs;
 
  const phaeton::Decl *Decl;

  const phaeton::Stmt *Stmt;

  const phaeton::Expression *Expr;

  const phaeton::ElementDirective *ElemDirective;

  const phaeton::Identifier *Identifier;
  const phaeton::Integer *Integer;
  const phaeton::BrackExpr *Brack;
  const phaeton::ParenExpr *Paren;

  const char *string_literal;
  int integer_literal;

  int InOutSpec;
  int PosSpec;

#line 126 "/Users/lijiansong/work-space/compile/git/phaeton/build/lib/Lang/phaeton.tab.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (yyscan_t yyscanner, const phaeton::Program *&root);

#endif /* !YY_YY_USERS_LIJIANSONG_WORK_SPACE_COMPILE_GIT_PHAETON_BUILD_LIB_LANG_PHAETON_TAB_HH_INCLUDED  */
