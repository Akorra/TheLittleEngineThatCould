# Collect all test files
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Create test executable
add_executable(TLETCTests ${TEST_SOURCES})

# Link against the library and Catch2
target_link_libraries(TLETCTests
    PRIVATE
        TLETC::TLETC
        Catch2::Catch2WithMain
)

# Set include directories if needed
target_include_directories(TLETCTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Include Catch2 CMake modules for test discovery
include(CTest)
include(Catch)

# Automatically discover tests
catch_discover_tests(TLETCTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    PROPERTIES
        LABELS "unit"
)

# Set output directory
set_target_properties(TLETCTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
    FOLDER "Tests"
)

# Organize test files in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Tests" FILES ${TEST_SOURCES})

message(STATUS "Tests configured - TLETCTests executable will be built")
