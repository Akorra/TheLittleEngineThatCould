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
add_tletc_example(01_MeshCreation        "01_MeshCreation/main.cpp")
add_tletc_example(02_SpinningCube        "02_SpinningCube/main.cpp")
add_tletc_example(03_Tesselation         "03_Tesselation/main.cpp")
add_tletc_example(04_PrimitiveTypes      "04_PrimitiveTypes/main.cpp")
add_tletc_example(05_InteractiveCamera   "05_InteractiveCamera/main.cpp")
add_tletc_example(06_OrderedPhases       "06_OrderedPhases/main.cpp")
add_tletc_example(07_TheLittleLocomotive "07_TheLittleLocomotive/main.cpp")
add_tletc_example(08_AllAboard           "08_AllAboard/main.cpp")

message(STATUS "Examples configured:")
message(STATUS "  - 01_MeshCreation")
message(STATUS "  - 02_SpinningCube")
message(STATUS "  - 03_Tesselation")
message(STATUS "  - 04_PrimitiveTypes")
message(STATUS "  - 05_InteractiveCamera")
message(STATUS "  - 06_OrderedPhases (Structured Game Loop)")
message(STATUS "  - 07_TheLittleLocomotive (Choo Choo!)")
message(STATUS "  - 08_AllAboard (Full Railroad Metaphor)")