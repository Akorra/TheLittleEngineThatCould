#include "TLETC/Core/Window.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/GeometryFactory.h"
#include "TLETC/Scene/Transform.h"

#include "../../src/Platform/OpenGL/GLRenderDevice.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

std::string LoadFileAsString(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " + path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() 
{
    std::cout << "========================================" << std::endl;
    std::cout << "  The Little Engine That Could" << std::endl;
    std::cout << "  Example: Spinning Cube" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Create window
    TLETC::Window window;
    if(!window.Create(1280, 720, "TLETC - Spinning Cube"))
    {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }

    std::cout << "Window created: " << window.GetWidth() << "x" << window.GetHeight() << std::endl;

    // Create render device
    TLETC::GLRenderDevice renderer;
    if (!renderer.Initialize()) 
    {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }

    std::cout << "Renderer: " << renderer.GetRendererName() << std::endl;
    std::cout << "OpenGL: " << renderer.GetAPIVersion() << std::endl;
    std::cout << std::endl;

    // Create a cube mesh
    TLETC::Mesh cube = TLETC::GeometryFactory::CreateCube(1.0f);
    std::cout << "Cube created:" << std::endl;
    std::cout << "  Vertices: " << cube.GetVertexCount() << std::endl;
    std::cout << "  Triangles: " << cube.GetTriangleCount() << std::endl;
    std::cout << std::endl;

    
    const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
    std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string() );
    std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

    // Create shader program
    auto vertexShader   = renderer.CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
    auto fragmentShader = renderer.CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
    auto shaderProgram  = renderer.CreateShaderProgram(vertexShader, fragmentShader);

    if (!shaderProgram.IsValid()) 
    {
        std::cerr << "Failed to create shader program!" << std::endl;
        return -1;
    }
    
    std::cout << "Shader program created successfully!" << std::endl;
    std::cout << std::endl;

    // Setup camera
    TLETC::Vec3 cameraPos(0.0f, 0.0f, 5.0f);
    TLETC::Mat4 view = TLETC::lookAt(cameraPos, TLETC::Vec3(0.0f), TLETC::Vec3(0.0f, 1.0f, 0.0f));
    TLETC::Mat4 projection = TLETC::perspective(TLETC::radians(45.0f), window.GetAspectRatio(), 0.1f, 100.0f);

    // Setup transform for the cube
    TLETC::Transform cubeTransform;

    // Lighting
    TLETC::Vec3 lightPos(2.0f, 2.0f, 2.0f);
    TLETC::Vec3 objectColor(0.2f, 0.6f, 1.0f); // Nice blue color

    std::cout << "Starting render loop..." << std::endl;
    std::cout << "Press ESC or close window to exit" << std::endl;
    std::cout << std::endl;

    double lastTime = window.GetTime();
    int frameCount = 0;

    // Main render loop
    while (!window.ShouldClose()) 
    {
        // Calculate delta time and FPS
        double currentTime = window.GetTime();
        double deltaTime = currentTime - lastTime;
        frameCount++;
        
        if (currentTime - lastTime >= 1.0) 
        {
            //std::cout << "FPS: " << frameCount << " | Frame time: " << (1000.0 / frameCount) << "ms" << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
        
        // Rotate the cube
        cubeTransform.Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), TLETC::Radians(50.0f));
        cubeTransform.Rotate(TLETC::Vec3(1.0f, 0.0f, 0.0f), TLETC::Radians(30.0f));
        
        // Clear the screen
        renderer.Clear(TLETC::Vec4(0.1f, 0.1f, 0.15f, 1.0f));
        
        // Use shader and set uniforms
        renderer.UseShader(shaderProgram);
        renderer.SetUniformMat4(shaderProgram, "u_view", view);
        renderer.SetUniformMat4(shaderProgram, "u_projection", projection);
        renderer.SetUniformVec3(shaderProgram, "u_lightPos", lightPos);
        renderer.SetUniformVec3(shaderProgram, "u_viewPos", cameraPos);
        renderer.SetUniformVec3(shaderProgram, "u_color", objectColor);
        
        // Draw the cube
        renderer.DrawMesh(cube, cubeTransform.GetModelMatrix());
        
        // Swap buffers and poll events
        window.SwapBuffers();
        window.PollEvents();
    }
    
    std::cout << std::endl;
    std::cout << "Cleaning up..." << std::endl;
    
    // Cleanup
    renderer.DestroyShader(shaderProgram);
    renderer.Shutdown();
    
    std::cout << "Done!" << std::endl;
    
    return 0;
}


