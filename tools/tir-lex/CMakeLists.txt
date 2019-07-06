set(TIR_LEXER_EXE tir-lex)
# Add incldue path for 'lex.yy.h' and 'tir.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(${TIR_LEXER_EXE} tir-lex.cpp)
target_link_libraries(${TIR_LEXER_EXE} Lexer)
