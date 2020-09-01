/*===--- pheaton.yy ----- Syntax rules for Phaeton ------------------------===*\
|*                                                                            *|
|*                     The Phaeton Compiler Infrastructure                    *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|*  This file defines and implements the syntax rules for DSL Phaeton.        *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

%define api.pure full

%param { yyscan_t yyscanner }
%parse-param { const phaeton::Program *&root }

%code requires {
  #include "ph/AST/AST.h"

  typedef void* yyscan_t;
}

%{
  #include <stdio.h>

  #include "phaeton.tab.hh"
  #include "lex.yy.h"

  void yyerror(yyscan_t scanner, const phaeton::Program *root, const char *msg) {
    fprintf(stderr, "%s\n", msg);
  }
%}

%union {
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
}

%token INT2
%token INT8
%token INT16
%token INT32
%token INT64
%token FLOAT8
%token FLOAT16
%token FLOAT32
%token FLOAT64
%token KW_VAR
%token KW_INPUT
%token KW_OUTPUT
%token KW_TYPE
%token SYMBOL_COLON
%token SYMBOL_LEFT_PAREN
%token SYMBOL_RIGHT_PAREN
%token SYMBOL_LEFT_BRACKET
%token SYMBOL_RIGHT_BRACKET
%token ARITHM_DOT
%token ARITHM_ADD
%token ARITHM_SUB
%token ARITHM_MUL
%token ARITHM_DIV
%token ARITHM_PRODUCT
%token EQUAL
%token KW_ELEM
%token KW_FIRST
%token KW_LAST
%token ARITHM_CARET
%token <integer_literal> INT
%token <string_literal> ID

%start program

%type <program> program
%type <Decls> decl_list
%type <Decl> decl var_decl type_decl
%type <Stmts> stmt_list
%type <Stmt> stmt
%type <Expr> type_expr expr term factor atom
%type <Identifier> identifier
%type <Integer> integer
%type <Brack> brack_expr
%type <Paren> paren_expr
%type <Exprs> expr_list
%type <ElemDirective> elem_direct
%type <InOutSpec> in_out_spec
%type <PosSpec> pos_spec

%%

/* BNF GRAMMAR RULES */

program : decl_list elem_direct stmt_list { root = phaeton::Program::create($1, $3, $2); }

decl_list : decl_list decl { $$ = phaeton::DeclList::append($1, $2); }
          | decl  { $$ = phaeton::DeclList::create($1); }

decl : var_decl
     | type_decl

var_decl : KW_VAR in_out_spec identifier SYMBOL_COLON type_expr {
             $$ = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_VarDecl,
                               $3, $5,
                               (phaeton::Decl::InOutSpecifier)$2);
           }

in_out_spec : /* empty */ { $$ = phaeton::Decl::IO_SPEC_Empty; }
       | KW_INPUT in_out_spec { $$ = phaeton::Decl::IO_SPEC_Input | $2; }
       | KW_OUTPUT in_out_spec { $$ = phaeton::Decl::IO_SPEC_Output | $2; }

type_decl : KW_TYPE identifier SYMBOL_COLON type_expr {
              $$ = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_TypeDecl, $2, $4);
            }

stmt_list : stmt_list stmt { $$ = phaeton::StmtList::append($1, $2); }
          | stmt { $$ = phaeton::StmtList::create($1); }

stmt : identifier EQUAL expr { $$ = phaeton::Stmt::create($1, $3); }

type_expr : identifier { $$ = (const phaeton::Expression *)$1; }
          | brack_expr { $$ = (const phaeton::Expression *)$1; }

expr : term
     | term ARITHM_ADD expr { $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_AddExpr, $1, $3); }
     | term ARITHM_SUB expr { $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_SubExpr, $1, $3); }

term : factor
     | factor ARITHM_MUL term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_MulExpr, $1, $3);
       }
     | factor ARITHM_DIV term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_DivExpr, $1, $3);
       }
     | factor ARITHM_DOT term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ContractionExpr, $1, $3);
       }

factor : atom
       | atom ARITHM_PRODUCT factor {
           $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ProductExpr, $1, $3);
         }
       | atom ARITHM_CARET factor {
           $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_TranspositionExpr, $1, $3);
         }

atom : identifier { $$ = (const phaeton::Expression *)$1; }
     | integer { $$ = (const phaeton::Expression *)$1; }
     | brack_expr { $$ = (const phaeton::Expression *)$1; }
     | paren_expr { $$ = (const phaeton::Expression *)$1; }

identifier : ID { $$ = phaeton::Identifier::create($1); }

integer : INT { $$ = phaeton::Integer::create($1); }
     
brack_expr : SYMBOL_LEFT_BRACKET expr_list SYMBOL_RIGHT_BRACKET { $$ = phaeton::BrackExpr::create($2); }
       
paren_expr : SYMBOL_LEFT_PAREN expr SYMBOL_RIGHT_PAREN { $$ = phaeton::ParenExpr::create($2); }

expr_list : /* empty */ { $$ = phaeton::ExprList::create(); }
          | expr_list expr {
              $$ = phaeton::ExprList::append($1, $2);
            }

pos_spec : KW_FIRST { $$ = phaeton::ElementDirective::POS_First; }
        | KW_LAST { $$ = phaeton::ElementDirective::POS_Last; }

elem_direct : /* empty */ { $$ = nullptr; }
            | KW_ELEM brack_expr integer pos_spec {
                $$ = phaeton::ElementDirective::create(phaeton::ASTNode::AST_NODE_KIND_ElementDirective,
                                        (phaeton::ElementDirective::POSSpecifier)$4, $3, $2);
              }

/* TODO: refine contraction op */
/*contract_expr : expr
              | expr ARITHM_DOT contract_expr {
                  $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ContractionExpr, $1, $3);
                }*/
/* TODO: add dnn ops */


%%
