GCC=clang
GXX=clang++
FLAGS:=-std=c++11

DEBUG=-g
LEXER_DEBUG=-D_LEXER_DEBUG_
PARSER_DEBUG=-D_PARSER_DEBUG_ -DYYDEBUG=1

LEXER=lexer
PARSER=parser
LEX=Lex
PARSE=Parse
COMPILE=Compile

all: $(LEXER) $(PARSER) $(LEX) $(PARSE) $(COMPILE)

$(LEXER): lex.yy.c lang.tab.cc
	$(GXX) $(DEBUG) $(LEXER_DEBUG) -o $@ $<

$(PARSER): lang.tab.cc lex.yy.c AST.o
	$(GXX) $(FLAGS) $(DEBUG) $(PARSER_DEBUG) -o $@ $+

$(LEX): lex.yy.c lang.tab.cc Lexer.o Lex.o
	$(GXX) $(DEBUG) -o $@ lex.yy.c Lexer.o Lex.o

$(PARSE): lex.yy.c lang.tab.cc Parse.o Lexer.o AST.o
	$(GXX) $(FLAGS) $(DEBUG) -o $@ $+

$(COMPILE): lang.tab.cc lex.yy.c Compile.o Lexer.o AST.o Sema.o
	$(GXX) $(FLAGS) $(DEBUG) -o $@ $+

lex.yy.c: lang.ll
	flex $<

lang.tab.cc: lang.yy
	bison $<

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) $(FLAGS) $(DEBUG) -c -o $@ $<

clean:
	@rm -f *.o lex.yy.c lang.tab.c lex.yy.h lang.tab.h lang.tab.hh lang.tab.cc
	@rm -rf *.dSYM
	@rm -f $(LEXER) $(PARSER) $(LEX) $(PARSE) $(COMPILE)

