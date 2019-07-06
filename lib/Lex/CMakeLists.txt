# Add include path for 'lex.yy.h' and 'tir.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

set_source_files_properties(${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.c GENERATED PROPERTIES LANGUAGE CXX)
set_source_files_properties(${TENSORIR_LANG_OUTPUT_DIR}/tir.tab.cc GENERATED)

add_library(Lexer STATIC Lexer.cpp
            ${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.c
            ${TENSORIR_LANG_OUTPUT_DIR}/tir.tab.cc)
add_dependencies(Lexer FlexLexerSources
                     BisonParserSources)
