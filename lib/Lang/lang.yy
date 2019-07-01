/* generate C++ output: */ 
/* %skeleton "lalr1.cc" */

/* generate header file: */

/* %define api.value.type variant */
/* generate a pure (i.e. re-entrant) parser: */
%define api.pure full

%param { yyscan_t yyscanner }
%parse-param {const Program *root }

%code requires {
  #include "tir/AST/AST.h"

  typedef void* yyscan_t;
}

%{

  #include <stdio.h>

  //#include "tir/AST/AST.h"

  //typedef void* yyscan_t;

  #include "lang.tab.hh"
  #include "lex.yy.h"

  //extern int yydebug;

  //extern FILE *yyin;

  //int yylex();
  //void yyerror(const char*);

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
  const Factor *factor;
  const BinaryExpr *binary;
  const Identifier *identifier;
  const Integer *integer;
  const BrackExpr *brack;
  const ParenExpr *paren;
  const char *string_literal;
  int integer_literal;
  int in_out_spec;
}

%token KW_VAR
%token KW_TYPE
%token KW_INPUT
%token KW_OUTPUT
%token COLON
%token LPAREN
%token RPAREN
%token LBRACK
%token RBRACK
%token STAR
%token DOT
%token EQUAL
%token <integer_literal> INT
%token <string_literal> ID

%start program

%type <program> program
%type <decls> decl_list
%type <decl> decl var_decl type_decl
%type <stmts> stmt_list
%type <stmt> stmt
%type <expr> type_expr expr
%type <factor> factor
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
         $$ = Decl::create(NT_VarDecl,
                           $3, $5,
                           (InOutSpecifier)$2);
         }

in_out_spec : /* empty */      { $$ = IO_Empty; }
       | KW_INPUT in_out_spec  { $$ = IO_Input | $2; }
       | KW_OUTPUT in_out_spec { $$ = IO_Output | $2; }

type_decl : KW_TYPE identifier COLON type_expr { $$ = Decl::create(NT_TypeDecl, $2, $4); }

stmt_list : stmt_list stmt { $$ = StmtList::append($1, $2); }
          | stmt { $$ = StmtList::create($1); }

stmt : identifier EQUAL expr { $$ = Stmt::create($1, $3); }

type_expr : expr

expr : expr STAR factor { $$ = BinaryExpr::create(NT_TensorExpr, $1, $3); }
     | expr DOT factor { $$ = BinaryExpr::create(NT_DotExpr, $1, $3); }
     | factor { $$ = (const Expr *)$1; }

factor : identifier { $$ = (const Factor *)$1; }
       | integer { $$ = (const Factor *)$1; }
       | brack_expr { $$ = (const Factor *)$1; }
       | paren_expr { $$ = (const Factor *)$1; }

identifier : ID { $$ = Identifier::create($1); }

integer : INT { $$ = Integer::create($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = BrackExpr::create($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = ParenExpr::create($2); }

expr_list : /* empty */ { $$ = ExprList::create(); }
          | expr_list expr { $$ = ExprList::append($1, $2); }

%%


#ifdef _PARSER_DEBUG_
#if 0
int main(int argc, char* argv[]) {
  FILE *in;
  yyscan_t scanner;
  int result;
  const Program *program;

  if (argc != 2) {
    return 1;
  } else {
    in = fopen(argv[1], "r");
    if (!in)
      return 2;
  }

  yydebug = 1;

  if (yylex_init(&scanner)) {
    return 3;
  }

  yyset_in(in, scanner);
  result = yyparse(scanner, program);

  fclose(in);
  yylex_destroy(scanner);

  if (result)
    return result;

  program->dump();
  Program::destroy(program);

  return 0;
}
#endif
#endif /* _PARSER_DEBUG_ */
