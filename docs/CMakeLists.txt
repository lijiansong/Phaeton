# Find doxygen.
find_package(Doxygen REQUIRED)

# Set the input and output documentation paths.
set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs)
# Configure the source Doxyfile by copying it and replacing all @variables@.
set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}/doxyfile)
configure_file(${DOXYGEN_INPUT_DIR}/doxygen.cfg.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

# Target setup.
add_custom_target(doc ALL
                  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
                  COMMAND doxygen ${DOXYGEN_CONFIGURED_INPUT}
                  COMMAND ${CMAKE_COMMAND} -E echo "Done."
                  WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR})
