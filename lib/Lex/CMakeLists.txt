# Add include path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

set_source_files_properties(${PHAETON_LANG_OUTPUT_DIR}/lex.yy.c GENERATED PROPERTIES LANGUAGE CXX)
set_source_files_properties(${PHAETON_LANG_OUTPUT_DIR}/phaeton.tab.cc GENERATED)

add_library(Lexer STATIC Lexer.cpp
    ${PHAETON_LANG_OUTPUT_DIR}/lex.yy.c
    ${PHAETON_LANG_OUTPUT_DIR}/phaeton.tab.cc)
add_dependencies(Lexer FlexLexerSources
                     BisonParserSources)
