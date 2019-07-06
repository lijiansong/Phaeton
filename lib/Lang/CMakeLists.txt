# bison version 2.7 or higher
find_package(BISON 2.7 REQUIRED)
# flex version 2.5 or higher
find_package(FLEX 2.5 REQUIRED)

add_custom_target(FlexLexerSources echo "Generating lex.yy.c lex.yy.h"
                  DEPENDS LangOutputDirectory)

add_custom_command(TARGET FlexLexerSources
                   COMMAND flex
                   ARGS --outfile=${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.c
                   --header-file=${TENSORIR_LANG_OUTPUT_DIR}/lex.yy.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/tir.ll
                   DEPENDS tir.ll)

add_custom_target(BisonParserSources echo "Generating tir.tab.cc tir.tab.hh"
                  DEPENDS LangOutputDirectory)

add_custom_command(TARGET BisonParserSources
                   COMMAND bison
                   ARGS --output=${TENSORIR_LANG_OUTPUT_DIR}/tir.tab.cc
                   --defines=${TENSORIR_LANG_OUTPUT_DIR}/tir.tab.hh
                        ${CMAKE_CURRENT_SOURCE_DIR}/tir.yy
                   DEPENDS tir.yy)
