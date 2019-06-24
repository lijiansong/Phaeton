# Add include path for 'lex.yy.h' and 'lang.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

set_source_files_properties(${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.c GENERATED PROPERTIES LANGUAGE CXX)
set_source_files_properties(${TENSORIR_LANG_OUTPUT_DIR}/lang.tab.cc GENERATED)

add_library(Parse STATIC
            Parser.cpp
            Lexer.cpp
            ${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.c
            ${TENSORIR_LANG_OUTPUT_DIR}/lang.tab.cc)
add_dependencies(Parse FlexLexerSources
                       BisonParserSources)
