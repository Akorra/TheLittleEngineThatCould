# Helper function to create examples
function(add_tletc_example EXAMPLE_NAME EXAMPLE_DIR)
    # Collect all source and header files recursively
    file(GLOB_RECURSE EXAMPLE_SOURCES 
        CONFIGURE_DEPENDS
        ${EXAMPLE_DIR}/*.cpp
        ${EXAMPLE_DIR}/*.c
        ${EXAMPLE_DIR}/*.hpp
        ${EXAMPLE_DIR}/*.h
    )

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCES})

    target_compile_definitions(${EXAMPLE_NAME} PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

    target_link_libraries(${EXAMPLE_NAME} PRIVATE TLETC::TLETC)

    # Make example root available for includes
    target_include_directories(${EXAMPLE_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_DIR}
    )

    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
        FOLDER "Examples"
    )
endfunction()

# Add examples
add_tletc_example(01_HelloWindow "01_HelloWindow")
add_tletc_example(02_MemoryTests "02_MemoryTests")
add_tletc_example(03_ECS         "03_ECS")
add_tletc_example(04_Systems     "04_Systems")
add_tletc_example(05_Hierarchy   "05_Hierarchy")
add_tletc_example(06_Events      "06_Events")

message(STATUS "Examples configured:")
message(STATUS "  - 01_HelloWindow")
message(STATUS "  - 02_MemoryTests")
message(STATUS "  - 03_ECS")
message(STATUS "  - 04_Systems")
message(STATUS "  - 05_Hierarchy")
message(STATUS "  - 06_Events")