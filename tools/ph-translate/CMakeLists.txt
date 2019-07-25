set(PH_COMPILER_EXE ph-translate)
# Add incldue path for 'lex.yy.h' and 'phaeton.tab.hh'
include_directories(${PHAETON_LANG_OUTPUT_DIR})

add_executable(${PH_COMPILER_EXE} ph-translate.cpp)
# FIXME: In CentOS, the linker order is necessary, 'Opt' must be behind 'CodeGen'
target_link_libraries(${PH_COMPILER_EXE} AST
                                         CodeGen
                                         Opt
                                         Lexer
                                         Parser
                                         Sema)
