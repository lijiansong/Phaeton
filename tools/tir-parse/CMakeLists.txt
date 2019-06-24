# Add incldue path for 'lex.yy.h' and 'lang.tab.hh'
include_directories(${TENSORIR_LANG_OUTPUT_DIR})

add_executable(tir-parse tir-parse.cpp)
target_link_libraries(tir-parse AST
                                Parse)
