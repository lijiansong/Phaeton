cmake_minimum_required(VERSION 3.5)

# A macro that helps to define an option
macro(phaeton_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Add an macro option for building the API documentations, default value is 'FALSE'.
phaeton_set_option(PHAETON_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it.")

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

if(PHAETON_BUILD_DOC)
    add_subdirectory(docs)
endif()

#add_subdirectory(tests)
#add_subdirectory(examples)
