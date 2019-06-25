# Add incldue path for 'lex.yy.h' and 'lang.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(tir-compile tir-compile.cpp)
target_link_libraries(tir-compile AST
                                  Parse
                                  Sema
                                  CodeGen)
