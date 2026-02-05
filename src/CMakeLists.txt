# Minimal source files for initial build test
set(TLETC_SOURCES
    Core/Math.cpp
    Core/Window.cpp
    Core/Input.cpp
    Core/Application.cpp
    Rendering/Handle.cpp
    # Rendering/Material.cpp
    Resources/Mesh.cpp
    Resources/GeometryFactory.cpp
    Scene/Entity.cpp
    Scene/Behaviour.cpp
    Platform/OpenGL/GLRenderDevice.cpp
)

# Public headers
set(TLETC_PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/TLETC/Core/Types.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Core/Math.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Core/Window.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Core/Input.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Core/Application.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Rendering/Handle.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Rendering/RenderDevice.h
    # ${CMAKE_SOURCE_DIR}/include/TLETC/Rendering/Material.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Scene/Transform.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Scene/Entity.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Scene/Behaviour.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Resources/Mesh.h
    ${CMAKE_SOURCE_DIR}/include/TLETC/Resources/GeometryFactory.h
)

# Create the library
if(TLETC_BUILD_SHARED)
    add_library(TLETC SHARED ${TLETC_SOURCES} ${TLETC_PUBLIC_HEADERS})
    target_compile_definitions(TLETC PRIVATE TLETC_BUILD_DLL)
    target_compile_definitions(TLETC PUBLIC TLETC_DLL)
    set_target_properties(TLETC PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
    )
else()
    add_library(TLETC STATIC ${TLETC_SOURCES} ${TLETC_PUBLIC_HEADERS})
endif()

# Add alias
add_library(TLETC::TLETC ALIAS TLETC)

# Set include directories (including GLM which is header-only)
target_include_directories(TLETC
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define GLM_ENABLE_EXPERIMENTAL to allow experimental features
target_compile_definitions(TLETC PUBLIC GLM_ENABLE_EXPERIMENTAL)

# Link dependencies
# Note: GLFW, GLAD, OpenGL are implementation details (PRIVATE)
# GLM is header-only and handled via include directories above
# Use BUILD_INTERFACE to prevent these from being exported

# Create a GLAD loader target for OpenGL core 3.3
glad_add_library(
    glad_gl_core_46
    REPRODUCIBLE
    API gl:core=4.6
)

target_link_libraries(TLETC
    PRIVATE
        $<BUILD_INTERFACE:OpenGL::GL>
        $<BUILD_INTERFACE:glfw>
        $<BUILD_INTERFACE:glad_gl_core_46>
)

# Install GLM headers alongside our headers so users don't need to find it separately
install(DIRECTORY ${glm_SOURCE_DIR}/glm
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.inl"
)

# Compiler warnings
if(MSVC)
    target_compile_options(TLETC PRIVATE /W4)
else()
    target_compile_options(TLETC PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Library properties
set_target_properties(TLETC PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "tletc"
    DEBUG_POSTFIX "d"
    FOLDER "TLETC"
)

# Organize in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TLETC_SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/include PREFIX "Header Files" FILES ${TLETC_PUBLIC_HEADERS})
