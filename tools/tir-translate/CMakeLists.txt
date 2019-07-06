set(TIR_TRANSLATE_EXE tir-translate)
# Add incldue path for 'lex.yy.h' and 'tir.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(${TIR_TRANSLATE_EXE} tir-translate.cpp)
target_link_libraries(${TIR_TRANSLATE_EXE} AST
                                           CodeGen
                                           Lexer
                                           Parser
                                           Sema)
