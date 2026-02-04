#include "TLETC/Core/Window.h"
#include "TLETC/Core/Input.h"
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

// simple FPS Camera
class FPSCamera 
{
public:
    TLETC::Vec3 position;
    float yaw;    // Left/Right
    float pitch;  // Up/Down
    float moveSpeed;
    float lookSpeed;

    FPSCamera() : position(0.0f, 2.0f, 5.0f), yaw(-90.0f), pitch(0.0f), moveSpeed(5.0f), lookSpeed(0.1f) {}

    void Update(const TLETC::Input& input, float deltaTime)
    {
        //Mouse Look
        if(input.IsCursorLocked())
        {
            TLETC::Vec2 mouseDelta = input.GetMouseDelta();
            yaw   += mouseDelta.x * lookSpeed;
            pitch -= mouseDelta.y * lookSpeed;
            
            // clamp pitch
            if(pitch >  89.0f) pitch =  89.0f;
            if(pitch < -89.0f) pitch = -89.0f;
        }

        // Calculate forward, right vectors
        TLETC::Vec3 forward;
        forward.x = TLETC::cos(TLETC::radians(yaw)) * TLETC::cos(TLETC::radians(pitch));
        forward.y = TLETC::sin(TLETC::radians(pitch));
        forward.z = TLETC::sin(TLETC::radians(yaw)) * TLETC::cos(TLETC::radians(pitch));
        forward   = TLETC::normalize(forward);

        TLETC::Vec3 right = TLETC::normalize(TLETC::cross(forward, TLETC::Vec3(0.0f, 1.0f, 0.0f)));
        TLETC::Vec3 up    = TLETC::normalize(TLETC::cross(right,   forward));

        // WASD movement
        float speed = moveSpeed * deltaTime;
        if(input.IsShiftPressed()) speed *= 2.0f; // sprint
        if (input.IsKeyPressed(TLETC::KeyCode::W))           position += forward * speed;
        if (input.IsKeyPressed(TLETC::KeyCode::S))           position -= forward * speed;
        if (input.IsKeyPressed(TLETC::KeyCode::A))           position -= right * speed;
        if (input.IsKeyPressed(TLETC::KeyCode::D))           position += right * speed;
        if (input.IsKeyPressed(TLETC::KeyCode::Space))       position += up * speed;
        if (input.IsKeyPressed(TLETC::KeyCode::LeftControl)) position -= up * speed;
    }

    TLETC::Mat4 GetViewMatrix() const 
    {
        TLETC::Vec3 forward;
        forward.x = TLETC::cos(TLETC::radians(yaw)) * TLETC::cos(TLETC::radians(pitch));
        forward.y = TLETC::sin(TLETC::radians(pitch));
        forward.z = TLETC::sin(TLETC::radians(yaw)) * TLETC::cos(TLETC::radians(pitch));
        forward = TLETC::normalize(forward);
        
        return TLETC::lookAt(position, position + forward, TLETC::Vec3(0.0f, 1.0f, 0.0f));
    }
};

int main() 
{
    std::cout << "========================================" << std::endl;
    std::cout << "  The Little Engine That Could" << std::endl;
    std::cout << "  Example: Interactive Camera" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Create Window
    TLETC::Window window;
    if (!window.Create(1280, 720, "TLETC - Interactive Camera")) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }
    
    // Create renderer
    TLETC::GLRenderDevice renderer;
    if (!renderer.Initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }

    // Initialize input
    TLETC::Input input;
    input.Initialize(window.GetNativeWindow());
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Mouse - Look around (click to lock cursor)" << std::endl;
    std::cout << "  Space - Move up" << std::endl;
    std::cout << "  Left Ctrl - Move down" << std::endl;
    std::cout << "  Left Shift - Sprint (move faster)" << std::endl;
    std::cout << "  ESC - Unlock cursor / Exit" << std::endl;
    std::cout << "  1-5 - Toggle wireframe" << std::endl;
    std::cout << std::endl;

    // Create meshes
    std::vector<TLETC::Mesh> meshes;
    meshes.push_back(TLETC::GeometryFactory::CreateCube(1.0f));
    meshes.push_back(TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16));
    meshes.push_back(TLETC::GeometryFactory::CreateCylinder(0.5f, 1.0f, 32));
    meshes.push_back(TLETC::GeometryFactory::CreateCone(0.5f, 1.0f, 32));
    meshes.push_back(TLETC::GeometryFactory::CreateTorus(0.5f, 0.2f, 32, 16));
    meshes.push_back(TLETC::GeometryFactory::CreateCapsule(0.25f, 0.5f, 32, 8));
    meshes.push_back(TLETC::GeometryFactory::CreateIcosphere(0.5f, 2));
    meshes.push_back(TLETC::GeometryFactory::CreatePlane(20.0f, 20.0f, 1, 1));

    const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
    std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string() );
    std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

    // Create shader
    auto vertShader = renderer.CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
    auto fragShader = renderer.CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
    auto program = renderer.CreateShaderProgram(vertShader, fragShader);

    if (!program.IsValid()) 
    {
        std::cerr << "Failed to create shader program!" << std::endl;
        return -1;
    }
    
    // Setup scene - grid of objects
    std::vector<TLETC::Transform> transforms;
    for (int x = -5; x <= 5; x += 2) 
    {
        for (int z = -5; z <= 5; z += 2) 
        {
            TLETC::Transform t;
            t.position = TLETC::Vec3(static_cast<float>(x), 0.5f, static_cast<float>(z));
            transforms.push_back(t);
        }
    }

    // Ground plane
    TLETC::Transform groundTransform;
    groundTransform.position = TLETC::Vec3(0.0f, 0.0f, 0.0f);
    
    // Setup camera
    FPSCamera camera;
    TLETC::Mat4 projection = TLETC::perspective(TLETC::radians(60.0f), window.GetAspectRatio(), 0.1f, 100.0f);

    TLETC::Vec3 lightPos(10.0f, 10.0f, 10.0f);
    
    bool wireframe = false;
    bool showInstructions = true;
    
    double lastTime = window.GetTime();
    double lastPrint = lastTime;
    int frameCount = 0;

    // Main loop
    while (!window.ShouldClose()) 
    {
        double currentTime = window.GetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        frameCount++;
        
        if (currentTime - lastPrint >= 1.0 && !showInstructions) 
        {
            std::cout << "FPS: " << frameCount 
                      << " | Pos: (" << camera.position.x << ", " 
                      << camera.position.y << ", " << camera.position.z << ")" << std::endl;
            frameCount = 0;
            lastPrint = currentTime;
        }

        // Update input
        input.Update();

        // ESC to unlock cursor or exit
        if (input.IsKeyJustPressed(TLETC::KeyCode::Escape)) 
        {
            if (input.IsCursorLocked()) 
            {
                input.SetCursorLocked(false);
                std::cout << "Cursor unlocked. Click window to lock again." << std::endl;
            } 
            else 
            {
                break; // Exit
            }
        }

        // Click to lock cursor
        if (input.IsMouseButtonJustPressed(TLETC::MouseButton::Left)) 
        {
            if (!input.IsCursorLocked()) 
            {
                input.SetCursorLocked(true);
                showInstructions = false;
                std::cout << "Cursor locked. Press ESC to unlock." << std::endl;
            }
        }

        // Toggle wireframe
        if (input.IsKeyJustPressed(TLETC::KeyCode::D1)) 
        {
            wireframe = !wireframe;
            std::cout << "Wireframe: " << (wireframe ? "ON" : "OFF") << std::endl;
        }

        // Update camera
        camera.Update(input, deltaTime);
        
        // Rotate objects
        for (auto& transform : transforms)
            transform.Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), 30.0f * deltaTime);

        // Render
        renderer.Clear(TLETC::Vec4(0.2f, 0.3f, 0.4f, 1.0f));
        renderer.SetWireframeMode(wireframe);

        TLETC::Mat4 view = camera.GetViewMatrix();
        
        renderer.UseShader(program);
        renderer.SetUniformMat4(program, "u_view", view);
        renderer.SetUniformMat4(program, "u_projection", projection);
        renderer.SetUniformVec3(program, "u_lightPos", lightPos);
        renderer.SetUniformVec3(program, "u_viewPos", camera.position);

        // Draw objects
        for (size_t i = 0; i < transforms.size(); ++i) 
        {
            int meshIndex = i % 7; // Cycle through first 7 meshes
            TLETC::Vec3 color(
                0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.5f),
                0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.7f + 2.0f),
                0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.9f + 4.0f)
            );
            renderer.SetUniformVec3(program, "u_color", color);
            renderer.DrawMesh(meshes[meshIndex], transforms[i].GetModelMatrix());
        }

        // Draw ground
        renderer.SetUniformVec3(program, "u_color", TLETC::Vec3(0.3f, 0.5f, 0.3f));
        renderer.DrawMesh(meshes[7], groundTransform.GetModelMatrix());
        
        renderer.SetWireframeMode(false);
        
        window.SwapBuffers();
        window.PollEvents();
    }

    std::cout << std::endl;
    std::cout << "Cleaning up..." << std::endl;
    
    input.Shutdown();
    renderer.DestroyShader(program);
    renderer.Shutdown();
    
    std::cout << "Done!" << std::endl;

    return 0;
}