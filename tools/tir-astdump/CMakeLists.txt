set(TIR_AST_DUMP_EXE tir-astdump)
# Add incldue path for 'lex.yy.h' and 'tir.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(${TIR_AST_DUMP_EXE} tir-astdump.cpp)
target_link_libraries(${TIR_AST_DUMP_EXE} AST
                                          Lexer
                                          Parser)
