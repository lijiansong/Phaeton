set(PH_LEXER_EXE ph-lex)
# Add incldue path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

add_executable(${PH_LEXER_EXE} ph-lex.cpp)
target_link_libraries(${PH_LEXER_EXE} Lexer)
