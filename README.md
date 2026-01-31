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
# Clone the repository
git clone https://github.com/Akorra/TheLittleEngineThatCould.git
cd TheLittleEngineThatCould

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Run examples
./bin/examples/HelloTriangle
```

### Building with Examples
```bash
cmake .. -DTLETC_BUILD_EXAMPLES=ON
cmake --build .
```

### Installation
```bash
# Install to system (might require sudo on Linux/macOS)
cmake --install . --prefix /usr/local

# Or install to custom location
cmake --install . --prefix ~/mylibs
```

## 📚 Usage

### Basic Example
```cpp
#include <TLETC/TLETC.h>

int main() {
    // Initialize engine
    TLETC::Engine engine;
    engine.Initialize(800, 600, "My First TLETC App");
    
    // Create a mesh
    TLETC::Mesh cubeMesh = TLETC::CreateCube();
    
    // Create a material
    TLETC::Material material;
    material.shader = "phong";
    material.diffuseColor = {1.0f, 0.0f, 0.0f, 1.0f};
    
    // Create an entity
    TLETC::Entity cube;
    cube.SetMesh(cubeMesh);
    cube.SetMaterial(material);
    cube.transform.position = {0.0f, 0.0f, -5.0f};
    
    // Add a rotation behavior
    cube.AddBehaviour<TLETC::RotatorBehaviour>(
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis
        45.0f                          // Degrees per second
    );
    
    // Main loop
    while (engine.IsRunning()) {
        engine.Update();
        engine.Render();
    }
    
    engine.Shutdown();
    return 0;
}
```

### Using in Your CMake Project

After installing TLETC, use it in your project:
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

# Find TLETC
find_package(TLETC REQUIRED)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE TLETC::TLETC)
```

Or use it as a subdirectory without installation:
```cmake
# Add TLETC as subdirectory
add_subdirectory(external/TheLittleEngineThatCould)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE TLETC::TLETC)
```

## 🏗️ Architecture

The engine is organized into several layers:
```
┌─────────────────────────────────────┐
│     Application / Game Logic        │
├─────────────────────────────────────┤
│    Scene (Entities, Behaviours)     │
├─────────────────────────────────────┤
│  Resources (Mesh, Material, Tex)    │
├─────────────────────────────────────┤
│      Rendering Abstraction          │
├─────────────────────────────────────┤
│   OpenGL │ Vulkan │ DirectX         │
│  (impl)  │ (TODO) │  (TODO)         │
└─────────────────────────────────────┘
```

### Key Components

- **Core**: Engine initialization, main loop, input handling
- **Resources**: API-agnostic data structures (meshes, materials, textures)
- **Rendering**: Abstract rendering interface and implementation-specific backends
- **Scene**: Entity system, transform hierarchy, behavior components
- **Math**: Vector, matrix, and quaternion operations (powered by GLM)

## 📖 Documentation

### Creating Custom Behaviors

Behaviors allow you to add custom logic to entities:
```cpp
class MyCustomBehaviour : public TLETC::Behaviour {
public:
    void OnInit() override {
        // Called when behavior is added to entity
    }
    
    void OnUpdate(float deltaTime) override {
        // Called every frame
        // Access entity with: GetEntity()
    }
    
    void OnPreRender() override {
        // Called before entity is rendered
    }
    
    void OnMouseDown(int x, int y, int button) override {
        // Handle mouse input
    }
};

// Usage
entity.AddBehaviour<MyCustomBehaviour>();
```

### Working with Materials
```cpp
TLETC::Material material;
material.shader = "phong";
material.diffuseColor = {1.0f, 0.0f, 0.0f, 1.0f};
material.specularColor = {1.0f, 1.0f, 1.0f, 1.0f};
material.shininess = 32.0f;
material.diffuseTexture = engine.LoadTexture("texture.png");
```

### Camera Setup
```cpp
TLETC::Entity camera;
camera.AddBehaviour<TLETC::CameraBehaviour>();
camera.AddBehaviour<TLETC::FPSCameraController>();
camera.transform.position = {0.0f, 0.0f, 5.0f};

engine.SetActiveCamera(&camera);
```

## 🗺️ Roadmap

### Version 0.1.0 (Current)
- [x] Basic OpenGL rendering
- [x] Mesh and material system
- [x] Entity and behavior framework
- [x] Camera implementation
- [x] CMake build system

### Version 0.2.0
- [ ] Lighting system (point, directional, spot lights)
- [ ] Shadow mapping
- [ ] Texture loading (PNG, JPG)
- [ ] Model loading (OBJ format)
- [ ] Basic UI system

### Version 0.3.0
- [ ] Vulkan backend
- [ ] Render graph system
- [ ] Advanced materials (PBR)
- [ ] Post-processing effects

### Version 1.0.0
- [ ] DirectX backend
- [ ] Physics integration
- [ ] Audio system
- [ ] Scene serialization
- [ ] Asset pipeline

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

### Development Setup
```bash
# Clone with submodules (if you add any git dependencies)
git clone --recursive https://github.com/yourusername/TheLittleEngineThatCould.git

# Build in debug mode for development
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DTLETC_BUILD_EXAMPLES=ON -DTLETC_BUILD_TESTS=ON
cmake --build .
```

### Code Style

- Use C++20 features where appropriate
- Follow the existing code style (roughly following Google C++ Style Guide)
- Add comments for complex logic
- Write clear commit messages

## 📝 Examples

Check out the `examples/` directory for complete working examples:

- **01_HelloTriangle**: Your first triangle with TLETC
- **02_RotatingCube**: A textured, rotating cube
- **03_MultipleObjects**: Scene with multiple entities
- **04_Lighting**: Basic lighting demonstration
- **05_Camera**: Camera controls and movement

## 🐛 Known Issues

- macOS: Some OpenGL deprecated warnings (OpenGL 4.1 max support)
- Linux: Requires X11 development libraries for window creation

See [Issues](https://github.com/yourusername/TheLittleEngineThatCould/issues) for more.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [GLFW](https://www.glfw.org/) - Window and input handling
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [GLM](https://glm.g-truc.net/) - Mathematics library
- [stb_image](https://github.com/nothings/stb) - Image loading (future)

## 📬 Contact

Filipe Marques - [@akorra07](https://www.linkedin.com/in/akorra07/)

Project Link: [https://github.com/Akorra/TheLittleEngineThatCould](https://github.com/Akorra/TheLittleEngineThatCould)

---

<p align="center">Made with ❤️ and a whole lot of OpenGL calls</p>
