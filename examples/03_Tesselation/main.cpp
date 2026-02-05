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
    std::cout << "  Example: Tessellated Terrain" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

      // Create window
    TLETC::Window window;
    if (!window.Create(1280, 720, "TLETC - Tessellated Terrain")) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }
    
    std::cout << "Window created" << std::endl;
    
    // Create render device
    TLETC::GLRenderDevice renderer;
    if (!renderer.Initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    std::cout << "Renderer: " << renderer.GetRendererName() << std::endl;
    std::cout << "OpenGL: " << renderer.GetAPIVersion() << std::endl;

    // Check for tessellation support
    int maxTessLevel = renderer.GetMaxTessLevel();
    std::cout << "Max tessellation level: " << maxTessLevel << std::endl;
    std::cout << std::endl;

    // Create a simple plane mesh
    TLETC::Mesh plane = TLETC::GeometryFactory::CreatePlane(4.0f, 4.0f, 1, 1);
    std::cout << "Plane created (will be tessellated on GPU)" << std::endl;
    std::cout << "  Base triangles: " << plane.GetTriangleCount() << std::endl;
    std::cout << std::endl;
    for(size_t i = 0; i<plane.GetVertexCount(); ++i)
    {
        auto pos = plane.GetVertexPosition(i);
        std::cout << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    }
    std::cout << std::endl;
    

    // Create tessellation shader program
    const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
    std::string tessVertexShader    = LoadFileAsString( (ProjectRoot / "examples/03_Tesselation/tesselation.vert").string() );
    std::string tessControlShader   = LoadFileAsString( (ProjectRoot / "examples/03_Tesselation/tesselation.cont").string() );
    std::string tessEvalShader      = LoadFileAsString( (ProjectRoot / "examples/03_Tesselation/tesselation.eval").string() );
    std::string tessFragmentShader  = LoadFileAsString( (ProjectRoot / "examples/03_Tesselation/tesselation.frag").string() );

    auto vertShader = renderer.CreateShader(TLETC::ShaderType::Vertex,        tessVertexShader);
    auto tcsShader = renderer.CreateShader(TLETC::ShaderType::TessControl,    tessControlShader);
    auto tesShader = renderer.CreateShader(TLETC::ShaderType::TessEvaluation, tessEvalShader);
    auto fragShader = renderer.CreateShader(TLETC::ShaderType::Fragment,      tessFragmentShader);
    
    if (!vertShader.IsValid() || !tcsShader.IsValid() || !tesShader.IsValid() || !fragShader.IsValid()) {
        std::cerr << "Failed to compile shaders!" << std::endl;
        std::cerr << "Your GPU may not support tessellation shaders (requires OpenGL 4.0+)" << std::endl;
        return -1;
    }

    auto shaderProgram = renderer.CreateShaderProgram(vertShader, tcsShader, tesShader, fragShader);
    
    if (!shaderProgram.IsValid()) {
        std::cerr << "Failed to create tessellation shader program!" << std::endl;
        return -1;
    }

    std::cout << "Tessellation shader program created!" << std::endl;
    std::cout << std::endl;

    // Setup camera
    TLETC::Vec3 cameraPos(0.0f, 5.0f, 5.0f);
    TLETC::Mat4 view = TLETC::lookAt(cameraPos, TLETC::Vec3(0.0f, 0.0f, 0.0f), TLETC::Vec3(0.0f, 1.0f, 0.0f));
    TLETC::Mat4 projection = TLETC::perspective(TLETC::radians(45.0f), window.GetAspectRatio(), 0.1f, 100.0f);
    
    TLETC::Transform planeTransform;
    TLETC::Vec3 lightPos(5.0f, 5.0f, 5.0f);

    float tessLevel = 8.0f;
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  Tessellation level: " << tessLevel << " (will animate)" << std::endl;
    std::cout << "  Press ESC or close window to exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting render loop..." << std::endl;

    double lastTime = window.GetTime();
    int frameCount = 0;
    
    // Enable wireframe to see tessellation
    renderer.SetWireframeMode(true);
    
    // Main render loop
    while (!window.ShouldClose()) 
    {
        double currentTime = window.GetTime();
        double deltaTime = currentTime - lastTime;
        frameCount++;
        
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << frameCount 
                      << " | Tess Level: " << static_cast<int>(tessLevel) << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
        
        // Animate tessellation level
        tessLevel = 4.0f + float(glm::sin(currentTime * 0.5f)) * 12.0f;
        if (tessLevel < 1.0f) tessLevel = 1.0f;
        
        // Rotate plane slowly
        planeTransform.Rotate(TLETC::Vec3(0,1,0), TLETC::Radians(10.0f));
        
        // Clear
        renderer.Clear(TLETC::Vec4(0.05f, 0.05f, 0.1f, 1.0f));
        
        // Use tessellation shader and set uniforms
        renderer.UseShader(shaderProgram);
        renderer.SetUniformMat4(shaderProgram,  "u_view", view);
        renderer.SetUniformMat4(shaderProgram,  "u_projection", projection);
        renderer.SetUniformFloat(shaderProgram, "u_tessLevel", tessLevel);
        renderer.SetUniformVec3(shaderProgram,  "u_lightPos", lightPos);
        renderer.SetUniformVec3(shaderProgram,  "u_viewPos", cameraPos);
        
        // Draw with tessellation (use Patches primitive type)
        // For now we'll modify DrawMesh to support tessellation
        // In a real implementation, you'd want a separate DrawTessellatedMesh method´
        renderer.SetPatchVertices(3);
        renderer.DrawMesh(plane, planeTransform.GetModelMatrix(), TLETC::PrimitiveType::Patches);
        
        window.SwapBuffers();
        window.PollEvents();
    }
    
    std::cout << std::endl;
    std::cout << "Cleaning up..." << std::endl;
    
    renderer.DestroyShader(shaderProgram);
    renderer.Shutdown();
    
    std::cout << "Done!" << std::endl;

    return 0;
}