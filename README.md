# The Little Engine That Could 🚂

A lightweight, modular 3D rendering engine designed for learning and experimentation. Built with modern C++ and designed with a clean abstraction layer to support multiple graphics APIs.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CMake](https://img.shields.io/badge/CMake-3.15+-blue.svg)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)

## ✨ Features

- **Clean Architecture**: Separation between geometry data, rendering state, and application logic
- **Multi-API Ready**: Designed with abstraction layers for future Vulkan and DirectX support
- **Behavior System**: Flexible component-based system for game logic, camera controls, and effects
- **Modern C++**: Built with C++17 features and best practices
- **Easy Integration**: CMake-based library that can be easily included in other projects
- **Cross-Platform**: Works on Windows, macOS, and Linux

### Current Status

- ✅ OpenGL rendering backend
- ✅ Basic mesh and material system
- ✅ Camera and transform components
- ✅ Behavior/component system
- 🚧 Vulkan backend (planned)
- 🚧 DirectX backend (planned)
- 🚧 Scene serialization (planned)

## 🚀 Quick Start

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- OpenGL 3.3+ capable graphics driver

### Building
```bash
