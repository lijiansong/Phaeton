set(PH_OPT_EXE ph-opt)
# Add incldue path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

add_executable(${PH_OPT_EXE} ph-opt.cpp)
target_link_libraries(${PH_OPT_EXE} AST
                                    Target
                                    CodeGen
                                    Opt
                                    Lexer
                                    Parser
                                    Sema)
