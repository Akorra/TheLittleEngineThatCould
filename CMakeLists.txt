cmake_minimum_required(VERSION 3.16)
project(TheLittleEngineThatCould 
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A modular 3D rendering engine"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(TLETC_BUILD_EXAMPLES "Build example programs" ON)
option(TLETC_BUILD_TESTS "Build tests" ON)
option(TLETC_BUILD_SHARED "Build shared library" OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add debug postfix for libraries
set(CMAKE_DEBUG_POSTFIX "d")

# Enable folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find dependencies
find_package(OpenGL REQUIRED)

# Use FetchContent for dependencies
include(FetchContent)

# GLFW for windowing
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLAD for OpenGL loading
FetchContent_Declare(
    glad2
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
)
FetchContent_MakeAvailable(glad2)

# Add the CMake folder from glad2 as a subdirectory
# GLAD2 includes its own CMake config for creating targets
add_subdirectory(
    ${glad2_SOURCE_DIR}/cmake
    ${glad2_BINARY_DIR}/cmake
)

# GLM for math
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# Catch2 for testing (only if building tests)
if(TLETC_BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

# Add the library
add_subdirectory(src)

# Add examples if requested
if(TLETC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Add tests if requested
if(TLETC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules
include(GNUInstallDirs)

# Install public headers
install(DIRECTORY include/TLETC
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install the library
install(TARGETS TLETC
    EXPORT TLETCTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT TLETCTargets
    FILE TLETCTargets.cmake
    NAMESPACE TLETC::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TLETC
)

# Create config files
include(CMakePackageConfigHelpers)

# Generate the version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/TLETCConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TLETCConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/TLETCConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TLETC
)

# Install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/TLETCConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/TLETCConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TLETC
)

# Export from build tree (commented out to avoid FetchContent target issues)
# Users should install the library or use add_subdirectory instead
# If you need build tree export, add GLFW/GLAD to their own export sets first
# export(EXPORT TLETCTargets
#     FILE "${CMAKE_CURRENT_BINARY_DIR}/TLETCTargets.cmake"
#     NAMESPACE TLETC::
# )

# Print configuration summary
message(STATUS "")
message(STATUS "=== TLETC Configuration Summary ===")
message(STATUS "Version:          ${PROJECT_VERSION}")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "Build examples:   ${TLETC_BUILD_EXAMPLES}")
message(STATUS "Build tests:      ${TLETC_BUILD_TESTS}")
message(STATUS "Build shared lib: ${TLETC_BUILD_SHARED}")
message(STATUS "Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
