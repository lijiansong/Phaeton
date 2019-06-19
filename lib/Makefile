GCC=clang
GXX=clang++

DEBUG=-g
LEXER_DEBUG=-D_LEXER_DEBUG_
PARSER_DEBUG=-D_PARSER_DEBUG_ -DYYDEBUG=1

LEXER=lexer
PARSER=parser

all: $(LEXER) $(PARSER)

$(LEXER): lex.yy.c
	$(GCC) $(DEBUG) $(LEXER_DEBUG) -o lexer lex.yy.c

$(PARSER): lang.tab.c lex.yy.c AST.o
	$(GXX) -std=c++11 $(DEBUG) $(PARSER_DEBUG) -o $@ $+

lex.yy.c: lang.ll
	flex -o $@ $<

lang.tab.c: lang.yy
	bison -o $@ $<

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) -std=c++11 $(DEBUG) -c -o $@ $<

clean:
	@rm -f *.o lex.yy.c lang.tab.c lang.tab.h lang.tab.hh lang.tab.cc lexer parser
	@rm -rf *.dSYM

