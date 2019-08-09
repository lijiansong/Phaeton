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
  
  phaeton::DeclList *decls;
  phaeton::StmtList *stmts;
  phaeton::ExprList *exprs;
 
  const phaeton::Decl *decl;

  const phaeton::Stmt *stmt;

  const phaeton::Expr *expr;

  const phaeton::ElementDirective *elem;

  const phaeton::Identifier *identifier;
  const phaeton::Integer *integer;
  const phaeton::BrackExpr *brack;
  const phaeton::ParenExpr *paren;

  const char *string_literal;
  int integer_literal;

  int in_out_spec;
  int pos_spec;
}

%token KW_VAR
%token KW_INPUT
%token KW_OUTPUT
%token KW_TYPE
%token COLON
%token LPAREN
%token RPAREN
%token LBRACK
%token RBRACK
%token DOT
%token ADD
%token SUB
%token MUL
%token DIV
%token PRODUCT
%token EQUAL
%token KW_ELEM
%token KW_FIRST
%token KW_LAST
%token CARET
%token <integer_literal> INT
%token <string_literal> ID

%start program

%type <program> program
%type <decls> decl_list
%type <decl> decl var_decl type_decl
%type <stmts> stmt_list
%type <stmt> stmt
%type <expr> type_expr expr term factor atom
%type <identifier> identifier
%type <integer> integer
%type <brack> brack_expr
%type <paren> paren_expr
%type <exprs> expr_list
%type <elem> elem_direct
%type <in_out_spec> in_out_spec
%type <pos_spec> pos_spec

%%

/* BNF GRAMMAR RULES */

program : decl_list elem_direct stmt_list { root = phaeton::Program::create($1, $3, $2); }

decl_list : decl_list decl { $$ = phaeton::DeclList::append($1, $2); }
          | decl  { $$ = phaeton::DeclList::create($1); }

decl : var_decl
     | type_decl

var_decl : KW_VAR in_out_spec identifier COLON type_expr {
             $$ = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_VarDecl,
                               $3, $5,
                               (phaeton::Decl::InOutSpecifier)$2);
           }

in_out_spec : /* empty */ { $$ = phaeton::Decl::IO_SPEC_Empty; }
       | KW_INPUT in_out_spec { $$ = phaeton::Decl::IO_SPEC_Input | $2; }
       | KW_OUTPUT in_out_spec { $$ = phaeton::Decl::IO_SPEC_Output | $2; }

type_decl : KW_TYPE identifier COLON type_expr {
              $$ = phaeton::Decl::create(phaeton::ASTNode::AST_NODE_KIND_TypeDecl, $2, $4);
            }

stmt_list : stmt_list stmt { $$ = phaeton::StmtList::append($1, $2); }
          | stmt { $$ = phaeton::StmtList::create($1); }

stmt : identifier EQUAL expr { $$ = phaeton::Stmt::create($1, $3); }

type_expr : identifier { $$ = (const phaeton::Expr *)$1; }
          | brack_expr { $$ = (const phaeton::Expr *)$1; }

expr : term
     | term ADD expr { $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_AddExpr, $1, $3); }
     | term SUB expr { $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_SubExpr, $1, $3); }

term : factor
     | factor MUL term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_MulExpr, $1, $3);
       }
     | factor DIV term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_DivExpr, $1, $3);
       }
     | factor DOT term {
         $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ContractionExpr, $1, $3);
       }

factor : atom
       | atom PRODUCT factor {
           $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ProductExpr, $1, $3);
         }
       | atom CARET factor {
           $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_TranspositionExpr, $1, $3);
         }

atom : identifier { $$ = (const phaeton::Expr *)$1; }
     | integer { $$ = (const phaeton::Expr *)$1; }
     | brack_expr { $$ = (const phaeton::Expr *)$1; }
     | paren_expr { $$ = (const phaeton::Expr *)$1; }

identifier : ID { $$ = phaeton::Identifier::create($1); }

integer : INT { $$ = phaeton::Integer::create($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = phaeton::BrackExpr::create($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = phaeton::ParenExpr::create($2); }

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
              | expr DOT contract_expr {
                  $$ = phaeton::BinaryExpr::create(phaeton::ASTNode::AST_NODE_KIND_ContractionExpr, $1, $3);
                }*/


%%
