#ifndef __TOKEN_H__
#define __TOKEN_H__

const int end_of_file = -1;

#ifdef _LEXER_DEBUG_
enum Token {
  KW_VAR,
  KW_TYPE,
  COLON,
  LPAREN,
  RPAREN,
  LBRACK,
  RBRACK,
  STAR,
  DOT,
  EQUAL,
  INT,
  ID,
  TOKEN_COUNT
};

char* token_string[TOKEN_COUNT] = {
  "KW_VAR",
  "KW_TYPE",
  "COLON",
  "LPAPREN",
  "RPAREN",
  "LBRACK",
  "RBRACK",
  "STAR",
  "DOT",
  "EQUAL",
  "INT",
  "ID"
};
#endif /* _LEXER_DEBUG_ */
#endif /* __TOKEN_H__ */
