cmake_minimum_required(VERSION 3.5)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(PHAETON_LANG_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib/Lang)
add_custom_target(LangOutputDirectory echo "Make directory: ${PHAETON_LANG_OUTPUT_DIR} ...")
add_custom_command(TARGET LangOutputDirectory
                   COMMAND mkdir
                   ARGS -p ${PHAETON_LANG_OUTPUT_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wno-deprecated-register -Wno-deprecated-declarations -Wno-deprecated")

include_directories(include)

add_subdirectory(lib)
add_subdirectory(tools)
#add_subdirectory(tests)
#add_subdirectory(examples)
