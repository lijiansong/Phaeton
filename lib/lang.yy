/* generate C++ output: */ 
/* %skeleton "lalr1.cc" */

/* generate header file: */
%defines

/* %define api.value.type variant */

%{

  #include <stdio.h>

  extern int yydebug;

  extern FILE *yyin;

  int yylex();
  void yyerror(const char*);

  void yyerror(const char *msg) {
    fprintf(stderr, "%s\n", msg);
  }

  #include "AST.h"

  const NODE_PTR root;
%}

%union {
  NODE_PTR node;
  const char *string;
  int integer;
}

%token KW_VAR
%token KW_TYPE
%token COLON
%token LPAREN
%token RPAREN
%token LBRACK
%token RBRACK
%token STAR
%token DOT
%token EQUAL
%token <integer> INT
%token <string> ID

%start program

%type <node> program
%type <node> decl_list decl var_decl type_decl
%type <node> stmt_list stmt
%type <node> type_expr expr factor identifier integer brack_expr paren_expr expr_list

%%

/* BNF GRAMMAR RULES */

program : decl_list stmt_list { root = createProgram($1, $2); }

decl_list : decl_list decl { $$ = appendDeclList($1, $2); }
          | decl  { $$ = createDeclList($1); }

decl : var_decl
     | type_decl

var_decl : KW_VAR identifier COLON type_expr { $$ = createVarDecl($2, $4); }

type_decl : KW_TYPE identifier COLON type_expr { $$ = createTypeDecl($2, $4); }

stmt_list : stmt_list stmt { $$ = appendStmtList($1, $2); }
          | stmt { $$ = createStmtList($1); }

stmt : identifier EQUAL expr { $$ = createStmt($1, $3); }

type_expr : expr

expr : expr STAR factor { $$ = createTensorExpr($1, $3); }
     | expr DOT factor { $$ = createDotExpr($1, $3); }
     | factor

factor : identifier
       | integer
       | brack_expr
       | paren_expr

identifier : ID { $$ = createIdentifier($1); }

integer : INT { $$ = createInteger($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = createBrackExpr($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = createParenExpr($2); }

expr_list : /* empty */ { $$ = createExprList(); }
          | expr_list expr { $$ = appendExprList($1, $2); }

%%


#ifdef _PARSER_DEBUG_

int main(int argc, char* argv[]) {
  FILE *in;
  int result;

  if (argc != 2) {
    return 1;
  } else {
    in = fopen(argv[1], "r");
    if (!in)
      return 2;
  }

  yydebug = 1;

  yyin = in;
  result = yyparse();

  fclose(in);

  if (result)
    return result;

  dumpAST(root);
  destroyAST(root);

  return 0;
}

#endif /* _PARSER_DEBUG_ */
