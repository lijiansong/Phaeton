set(PH_TRANSLATE_EXE ph-translate)
# Add incldue path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

add_executable(${PH_TRANSLATE_EXE} ph-translate.cpp)
target_link_libraries(${PH_TRANSLATE_EXE} AST
                                          Opt
                                          CodeGen
                                          Lexer
                                          Parser
                                          Sema)
