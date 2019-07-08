set(TIR_OPT_EXE tir-opt)
# Add incldue path for 'lex.yy.h' and 'tir.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(${TIR_OPT_EXE} tir-opt.cpp)
target_link_libraries(${TIR_OPT_EXE} AST
                                     CodeGen
                                     Lexer
                                     Parser
                                     Sema)
