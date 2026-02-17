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
add_tletc_example(01_HelloWindow "01_HelloWindow/main.cpp")
add_tletc_example(02_MemoryTests "02_MemoryTests/main.cpp")
add_tletc_example(03_ECS "03_ECS/main.cpp")

message(STATUS "Examples configured:")
message(STATUS "  - 01_HelloWindow")
message(STATUS "  - 02_MemoryTests")
message(STATUS "  - 03_ECS")