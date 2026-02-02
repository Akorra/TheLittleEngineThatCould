# Standard build
mkdir build && cd build
cmake ..
cmake --build .

# With presets (if using CMakePresets.json)
cmake --preset debug
cmake --build --preset debug
ctest --preset debug

# Release build
cmake --preset release
cmake --build --preset release

# Library only (no examples or tests)
cmake --preset lib-only
cmake --build --preset lib-only

# With specific options
cmake .. -DTLETC_BUILD_EXAMPLES=OFF -DTLETC_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Install
cmake --install . --prefix /usr/local

# Or with custom prefix
cmake --install . --prefix ~/mylibs


# Example Usage in Another Project
# Once your library is built and installed, other projects can use it like this:
# ExternalProject/CMakeLists.txt:
cmakecmake_minimum_required(VERSION 3.15)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

# Find the installed library
find_package(TLETC REQUIRED)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE TLETC::TLETC)

# Building the Library
# bash# Clone and enter the directory
cd TheLittleEngineThatCould

# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Install (optional, might need sudo on Linux)
cmake --install . --prefix /usr/local

# Or install to a custom location
cmake --install . --prefix ~/mylibs

# Building with Examples
bashcmake .. -DTLETC_BUILD_EXAMPLES=ON
cmake --build .
./bin/examples/HelloTriangle