#include "TLETC/Core/Math.h"
#include "TLETC/Core/Application.h"
#include "TLETC/Scene/Behaviour.h"
#include "TLETC/Resources/GeometryFactory.h"

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

// Phase-aware behaviours that demonstrate update ordering

/**
 * InputHandler - Handles input in EarlyUpdate (before physics)
 */
class InputHandler : public TLETC::Behaviour 
{
public:
    TLETC::Vec3 velocity{0.0f};
    float moveSpeed = 5.0f;

    InputHandler() { SetActiveEvents(EventFlag::EarlyUpdate); }
    
    void OnEarlyUpdate(float deltaTime) override 
    {
        // Read input FIRST, before physics/logic
        auto* input = GetInput();
        if (!input) return;
        
        velocity = TLETC::Vec3(0.0f);
        
        if (input->IsKeyPressed(TLETC::KeyCode::W)) velocity.z -= 1.0f;
        if (input->IsKeyPressed(TLETC::KeyCode::S)) velocity.z += 1.0f;
        if (input->IsKeyPressed(TLETC::KeyCode::A)) velocity.x -= 1.0f;
        if (input->IsKeyPressed(TLETC::KeyCode::D)) velocity.x += 1.0f;
        
        if (TLETC::length(velocity) > 0.0f) {
            velocity = TLETC::normalize(velocity) * moveSpeed;
        }
    }
    
    const char* GetName() const override { return "InputHandler"; }
};

/**
 * PhysicsMover - Applies velocity in Update (main physics phase)
 */
class PhysicsMover : public TLETC::Behaviour 
{
public:
    PhysicsMover() { { SetActiveEvents(EventFlag::Update); } }

    void OnUpdate(float deltaTime) override 
    {
        // Apply physics AFTER input is read
        auto* inputHandler = GetEntity()->GetBehaviour<InputHandler>();
        if (inputHandler)
            GetEntity()->transform.position += inputHandler->velocity * deltaTime;
    }
    
    const char* GetName() const override { return "PhysicsMover"; }
};

/**
 * CameraFollow - Follows target in LateUpdate (after all movement)
 */
class CameraFollow : public TLETC::Behaviour 
{
public:
    TLETC::Entity* target = nullptr;
    TLETC::Vec3 offset{0, 5, 10};
    float smoothSpeed = 5.0f;

    CameraFollow() { { SetActiveEvents(EventFlag::LateUpdate); }}
    
    void OnLateUpdate(float deltaTime) override {
        // Update camera AFTER all movement is finalized
        if (target) 
        {
            TLETC::Vec3 desiredPos = target->transform.position + offset;
            GetEntity()->transform.position = TLETC::mix(
                GetEntity()->transform.position,
                desiredPos,
                smoothSpeed * deltaTime
            );
        }
    }
    
    const char* GetName() const override { return "CameraFollow"; }
};

/**
 * Rotator - Simple rotation
 */
class Rotator : public TLETC::Behaviour 
{
public:
    TLETC::Vec3 axis;
    float speed;
    
    Rotator(const TLETC::Vec3& a = TLETC::Vec3(0, 1, 0), float s = 45.0f) : axis(a), speed(s) { SetActiveEvents(EventFlag::Update); }
    
    void OnUpdate(float deltaTime) override
    {
        GetEntity()->transform.Rotate(axis, speed * deltaTime);
    }
    
    const char* GetName() const override { return "Rotator"; }
};

/**
 * PhaseLogger - Logs which phase we're in (for demonstration)
 */
class PhaseLogger : public TLETC::Behaviour 
{
public:
    bool enabled = false;

    PhaseLogger() { SetActiveEvents(EventFlag::AllRender | EventFlag::AllRender | EventFlag::KeyEvents); }
    
    void OnKeyPressed(TLETC::KeyCode key) override { if(key == TLETC::KeyCode::L) enabled = !enabled; }
    void OnEarlyUpdate(float dt) override 
    {
        if (enabled) std::cout << "  [EarlyUpdate] " << GetEntity()->name << std::endl;
    }
    
    void OnUpdate(float dt) override 
    {
        if (enabled) std::cout << "  [Update] " << GetEntity()->name << std::endl;
    }
    
    void OnLateUpdate(float dt) override 
    {
        if (enabled) std::cout << "  [LateUpdate] " << GetEntity()->name << std::endl;
    }
    
    void OnPreRender() override 
    {
        if (enabled) std::cout << "  [PreRender] " << GetEntity()->name << std::endl;
    }
    
    void OnPostRender() override 
    {
        if (enabled) std::cout << "  [PostRender] " << GetEntity()->name << std::endl;
    }
    
    const char* GetName() const override { return "PhaseLogger"; }
};

// Custom game application
class MyGame : public TLETC::Application 
{
public:
    MyGame() : Application("Ordered Update Phases Demo", 1280, 720) {}
    
    void OnInit() override {
        std::cout << "========================================" << std::endl;
        std::cout << "  Ordered Update Phases Demo" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Update phases execute in order:" << std::endl;
        std::cout << "  1. ProcessInput  - Read hardware" << std::endl;
        std::cout << "  2. EarlyUpdate   - Handle input" << std::endl;
        std::cout << "  3. Update        - Main logic/physics" << std::endl;
        std::cout << "  4. LateUpdate    - Cameras, etc." << std::endl;
        std::cout << "  5. PreRender     - Prepare rendering" << std::endl;
        std::cout << "  6. Render        - Draw" << std::endl;
        std::cout << "  7. PostRender    - UI, cleanup" << std::endl;
        std::cout << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move player (blue cube)" << std::endl;
        std::cout << "  L - Toggle phase logging" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        std::cout << std::endl;
        
        // Create meshes
        cubeMesh = TLETC::GeometryFactory::CreateCube(1.0f);
        sphereMesh = TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16);
        
        // Create player
        player = CreateEntity("Player");
        player->mesh = &cubeMesh;
        player->transform.position = TLETC::Vec3(0, 0, 0);
        player->AddBehaviour<InputHandler>();    // Reads input in EarlyUpdate
        player->AddBehaviour<PhysicsMover>();    // Applies movement in Update
        playerLogger = player->AddBehaviour<PhaseLogger>();
        
        // Create camera entity
        camera = CreateEntity("Camera");
        camera->transform.position = TLETC::Vec3(0, 5, 10);
        auto* follow = camera->AddBehaviour<CameraFollow>();
        follow->target = player;
        cameraLogger = camera->AddBehaviour<PhaseLogger>();
        
        // Create some decoration
        for (int i = 0; i < 5; ++i) 
        {
            auto* entity = CreateEntity("Spinner" + std::to_string(i));
            entity->mesh = &sphereMesh;
            entity->transform.position = TLETC::Vec3(std::cos(i * 1.2f) * 5.0f, 0, std::sin(i * 1.2f) * 5.0f);
            entity->AddBehaviour<Rotator>(TLETC::Vec3(0, 1, 0), 90.0f);
        }
        
        const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
        std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string() );
        std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

        // Create shaders
        auto* renderer = GetRenderDevice();
        auto vertShader = renderer->CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
        auto fragShader = renderer->CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
        shaderProgram = renderer->CreateShaderProgram(vertShader, fragShader);
        
        // Setup projection
        projection = TLETC::perspective(TLETC::radians(45.0f), GetWindow().GetAspectRatio(), 0.1f, 100.0f );
    }
    
    void OnRender() override 
    {
        auto* renderer = GetRenderDevice();
        
        // Clear
        renderer->Clear(TLETC::Vec4(0.1f, 0.1f, 0.15f, 1.0f));
        
        // Setup shader
        renderer->UseShader(shaderProgram);
        
        TLETC::Mat4 view = TLETC::lookAt(camera->transform.position, player->transform.position, TLETC::Vec3(0, 1, 0));
        
        renderer->SetUniformMat4(shaderProgram, "u_view", view);
        renderer->SetUniformMat4(shaderProgram, "u_projection", projection);
        renderer->SetUniformVec3(shaderProgram, "u_lightPos", TLETC::Vec3(5, 5, 5));
        renderer->SetUniformVec3(shaderProgram, "u_viewPos", camera->transform.position);
        
        // Draw all entities
        for (const auto& entity : GetEntities()) 
        {
            if (entity->mesh) 
            {
                TLETC::Vec3 color = (entity.get() == player) ? TLETC::Vec3(0.3f, 0.5f, 1.0f) : TLETC::Vec3(1.0f, 0.3f, 0.3f);
                renderer->SetUniformVec3(shaderProgram, "u_color", color);
                renderer->DrawMesh(*entity->mesh, entity->transform.GetModelMatrix());
            }
        }
    }
    
    void OnShutdown() override 
    {
        GetRenderDevice()->DestroyShader(shaderProgram);
    }
    
private:
    TLETC::Mesh cubeMesh, sphereMesh;
    TLETC::Entity* player;
    TLETC::Entity* camera;
    TLETC::ShaderHandle shaderProgram;
    TLETC::Mat4 projection;
    PhaseLogger* playerLogger;
    PhaseLogger* cameraLogger;
    bool loggingEnabled = false;
};

int main() {
    MyGame game;
    
    if (!game.Initialize()) {
        return -1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
}