cmake_minimum_required(VERSION 3.5)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(TENSORIR_LANG_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib/Lang)
add_custom_target(LangOutputDirectory echo "Make directory: ${TENSORIR_LANG_OUTPUT_DIR} ...")
add_custom_command(TARGET LangOutputDirectory
                   COMMAND mkdir
                   ARGS -p ${TENSORIR_LANG_OUTPUT_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

include_directories(include)

add_subdirectory(lib)
add_subdirectory(tools)
#add_subdirectory(tests)
