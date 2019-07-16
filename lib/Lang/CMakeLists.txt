# bison version 2.7 or higher
find_package(BISON 2.7 REQUIRED)
# flex version 2.5 or higher
find_package(FLEX 2.5 REQUIRED)

add_custom_target(FlexLexerSources echo "Generating lex.yy.c lex.yy.h"
                  DEPENDS LangOutputDirectory)

add_custom_command(TARGET FlexLexerSources
                   COMMAND flex
                   ARGS --outfile=${PHAETON_LANG_OUTPUT_DIR}/lex.yy.c
                   --header-file=${PHAETON_LANG_OUTPUT_DIR}/lex.yy.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/phaeton.ll
                   DEPENDS phaeton.ll)

add_custom_target(BisonParserSources echo "Generating phaeton.tab.cc phaeton.tab.hh"
                  DEPENDS LangOutputDirectory)

add_custom_command(TARGET BisonParserSources
                   COMMAND bison
                   ARGS --output=${PHAETON_LANG_OUTPUT_DIR}/phaeton.tab.cc
                   --defines=${PHAETON_LANG_OUTPUT_DIR}/phaeton.tab.hh
                        ${CMAKE_CURRENT_SOURCE_DIR}/phaeton.yy
                   DEPENDS phaeton.yy)
