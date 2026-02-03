#include "TLETC/Core/Window.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/GeometryFactory.h"
#include "TLETC/Scene/Transform.h"

#include "../../src/Platform/OpenGL/GLRenderDevice.h"

#include <iostream>

// Simple shaders
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_color;

void main() {
    v_normal = normalize(mat3(u_model) * a_normal);
    v_color = u_color;
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 v_normal;
in vec3 v_color;

void main() {
    // Simple lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(v_normal, lightDir), 0.0);
    vec3 result = v_color * (0.3 + 0.7 * diff);
    FragColor = vec4(result, 1.0);
}
)";

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  The Little Engine That Could" << std::endl;
    std::cout << "  Example: Primitive Types & Wireframe" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Create window
    TLETC::Window window;
    if (!window.Create(1280, 720, "TLETC - Primitive Types")) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }
    
    // Create renderer
    TLETC::GLRenderDevice renderer;
    if (!renderer.Initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    std::cout << "Renderer: " << renderer.GetRendererName() << std::endl;
    std::cout << std::endl;
    
    // Create meshes
    TLETC::Mesh sphere = TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16);
    TLETC::Mesh cube = TLETC::GeometryFactory::CreateCube(1.0f);
    TLETC::Mesh torus = TLETC::GeometryFactory::CreateTorus(0.5f, 0.2f, 32, 16);
    
    std::cout << "Meshes created:" << std::endl;
    std::cout << "  Sphere: " << sphere.GetVertexCount() << " vertices" << std::endl;
    std::cout << "  Cube: " << cube.GetVertexCount() << " vertices" << std::endl;
    std::cout << "  Torus: " << torus.GetVertexCount() << " vertices" << std::endl;
    std::cout << std::endl;
    
    // Create shader
    auto vertShader = renderer.CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
    auto fragShader = renderer.CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
    auto program = renderer.CreateShaderProgram(vertShader, fragShader);
    
    if (!program.IsValid()) {
        std::cerr << "Failed to create shader program!" << std::endl;
        return -1;
    }
    
    // Setup camera
    TLETC::Mat4 projection = TLETC::perspective(
        TLETC::radians(45.0f),
        window.GetAspectRatio(),
        0.1f,
        100.0f
    );
    
    // Setup transforms for 4 objects
    TLETC::Transform transforms[4];
    transforms[0].position = TLETC::Vec3(-2.5f, 1.0f, 0.0f);  // Top left
    transforms[1].position = TLETC::Vec3( 2.5f, 1.0f, 0.0f);  // Top right
    transforms[2].position = TLETC::Vec3(-2.5f, -1.0f, 0.0f); // Bottom left
    transforms[3].position = TLETC::Vec3( 2.5f, -1.0f, 0.0f); // Bottom right
    
    TLETC::Vec3 colors[4] = {
        TLETC::Vec3(1.0f, 0.3f, 0.3f), // Red
        TLETC::Vec3(0.3f, 1.0f, 0.3f), // Green
        TLETC::Vec3(0.3f, 0.3f, 1.0f), // Blue
        TLETC::Vec3(1.0f, 1.0f, 0.3f)  // Yellow
    };
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  Mode cycles automatically every 2 seconds" << std::endl;
    std::cout << "  Top Left:     Solid Triangles" << std::endl;
    std::cout << "  Top Right:    Wireframe" << std::endl;
    std::cout << "  Bottom Left:  Points" << std::endl;
    std::cout << "  Bottom Right: Lines" << std::endl;
    std::cout << std::endl;
    
    double lastTime = window.GetTime();
    int frameCount = 0;
    double lastPrint = lastTime;
    
    // Main loop
    while (!window.ShouldClose()) {
        double currentTime = window.GetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        frameCount++;
        
        if (currentTime - lastPrint >= 1.0) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastPrint = currentTime;
        }
        
        // Rotate all objects
        for (int i = 0; i < 4; ++i) {
            transforms[i].Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), 30.0f * static_cast<float>(deltaTime));
            transforms[i].Rotate(TLETC::Vec3(1.0f, 0.0f, 0.0f), 20.0f * static_cast<float>(deltaTime));
        }
        
        // Orbit camera
        float camAngle = static_cast<float>(currentTime * 0.3f);
        TLETC::Vec3 cameraPos(
            std::sin(camAngle) * 8.0f,
            3.0f,
            std::cos(camAngle) * 8.0f
        );
        TLETC::Mat4 view = TLETC::lookAt(cameraPos, TLETC::Vec3(0.0f), TLETC::Vec3(0.0f, 1.0f, 0.0f));
        
        // Cycle through meshes every 2 seconds
        int meshIndex = static_cast<int>(currentTime / 2.0) % 3;
        const TLETC::Mesh* currentMesh = nullptr;
        switch (meshIndex) {
            case 0: currentMesh = &sphere; break;
            case 1: currentMesh = &cube; break;
            case 2: currentMesh = &torus; break;
        }
        
        // Clear
        renderer.Clear(TLETC::Vec4(0.1f, 0.1f, 0.15f, 1.0f));
        
        // Setup shader
        renderer.UseShader(program);
        renderer.SetUniformMat4(program, "u_view", view);
        renderer.SetUniformMat4(program, "u_projection", projection);
        
        // Draw 4 objects with different primitive types
        
        // Top Left: Solid Triangles (default)
        renderer.SetWireframeMode(false);
        renderer.SetUniformVec3(program, "u_color", colors[0]);
        renderer.DrawMesh(*currentMesh, transforms[0].GetModelMatrix(), TLETC::PrimitiveType::Triangles);
        
        // Top Right: Wireframe
        renderer.SetWireframeMode(true);
        renderer.SetUniformVec3(program, "u_color", colors[1]);
        renderer.DrawMesh(*currentMesh, transforms[1].GetModelMatrix(), TLETC::PrimitiveType::Triangles);
        renderer.SetWireframeMode(false);
        
        // Bottom Left: Points
        renderer.SetUniformVec3(program, "u_color", colors[2]);
        renderer.DrawMesh(*currentMesh, transforms[2].GetModelMatrix(), TLETC::PrimitiveType::Points);
        
        // Bottom Right: Lines (wireframe via Lines primitive)
        renderer.SetUniformVec3(program, "u_color", colors[3]);
        renderer.DrawMesh(*currentMesh, transforms[3].GetModelMatrix(), TLETC::PrimitiveType::Lines);
        
        window.SwapBuffers();
        window.PollEvents();
    }
    
    std::cout << std::endl;
    std::cout << "Cleaning up..." << std::endl;
    
    renderer.DestroyShader(program);
    renderer.Shutdown();
    
    std::cout << "Done!" << std::endl;
    
    return 0;
}