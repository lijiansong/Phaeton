set(PH_AST_DUMP_EXE ph-astdump)
# Add incldue path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

add_executable(${PH_AST_DUMP_EXE} ph-astdump.cpp)
target_link_libraries(${PH_AST_DUMP_EXE} AST
                                          Lexer
                                          Parser)
