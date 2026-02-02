# Helper function to create examples
function(add_tletc_example EXAMPLE_NAME SOURCE_FILE)
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILE})
    target_compile_definitions(${EXAMPLE_NAME} PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
    target_link_libraries(${EXAMPLE_NAME} PRIVATE TLETC::TLETC)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
        FOLDER "Examples"
    )
endfunction()

# Add examples
add_tletc_example(01_MeshCreation "01_MeshCreation/main.cpp")
add_tletc_example(02_SpinningCube "02_SpinningCube/main.cpp")

message(STATUS "Examples configured:")
message(STATUS "  - 01_MeshCreation")
message(STATUS "  - 02_SpinningCube")