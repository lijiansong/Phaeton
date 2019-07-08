%define api.pure full

%param { yyscan_t yyscanner }
%parse-param { const Program *&root }

%code requires {
  #include "tir/AST/AST.h"

  typedef void* yyscan_t;
}

%{
  #include <stdio.h>


  #include "tir.tab.hh"
  #include "lex.yy.h"

  void yyerror(yyscan_t scanner, const Program *root, const char *msg) {
    fprintf(stderr, "%s\n", msg);
  }
%}

%union {
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
%type <in_out_spec> in_out_spec

%%

/* BNF GRAMMAR RULES */

program : decl_list stmt_list { root = Program::create($1, $2); }

decl_list : decl_list decl { $$ = DeclList::append($1, $2); }
          | decl  { $$ = DeclList::create($1); }

decl : var_decl
     | type_decl

var_decl : KW_VAR in_out_spec identifier COLON type_expr {
             $$ = Decl::create(ASTNode::NT_VarDecl,
                               $3, $5,
                               (Decl::InOutSpecifier)$2);
           }

in_out_spec : /* empty */ { $$ = Decl::IO_Empty; }
       | KW_INPUT in_out_spec { $$ = Decl::IO_Input | $2; }
       | KW_OUTPUT in_out_spec { $$ = Decl::IO_Output | $2; }

type_decl : KW_TYPE identifier COLON type_expr {
              $$ = Decl::create(ASTNode::NT_TypeDecl, $2, $4);
            }

stmt_list : stmt_list stmt { $$ = StmtList::append($1, $2); }
          | stmt { $$ = StmtList::create($1); }

stmt : identifier EQUAL expr { $$ = Stmt::create($1, $3); }

type_expr : identifier { $$ = (const Expr *)$1; }
          | brack_expr { $$ = (const Expr *)$1; }

/* TODO: refine contraction op */
/*contract_expr : expr
              | expr DOT contract_expr {
                  $$ = BinaryExpr::create(ASTNode::NT_ContractionExpr, $1, $3);
                }*/

expr : term
     | term ADD expr { $$ = BinaryExpr::create(ASTNode::NT_AddExpr, $1, $3); }
     | term SUB expr { $$ = BinaryExpr::create(ASTNode::NT_SubExpr, $1, $3); }

term : factor
     | factor MUL term {
         $$ = BinaryExpr::create(ASTNode::NT_MulExpr, $1, $3);
       }
     | factor DIV term {
         $$ = BinaryExpr::create(ASTNode::NT_DivExpr, $1, $3);
       }
     | factor DOT term {
          $$ = BinaryExpr::create(ASTNode::NT_ContractionExpr, $1, $3);
       }

factor : atom
       | atom PRODUCT factor {
           $$ = BinaryExpr::create(ASTNode::NT_ProductExpr, $1, $3);
         }

atom : identifier { $$ = (const Expr *)$1; }
     | integer { $$ = (const Expr *)$1; }
     | brack_expr { $$ = (const Expr *)$1; }
     | paren_expr { $$ = (const Expr *)$1; }

identifier : ID { $$ = Identifier::create($1); }

integer : INT { $$ = Integer::create($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = BrackExpr::create($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = ParenExpr::create($2); }

expr_list : /* empty */ { $$ = ExprList::create(); }
          | expr_list expr {
              $$ = ExprList::append($1, $2);
          }

%%
