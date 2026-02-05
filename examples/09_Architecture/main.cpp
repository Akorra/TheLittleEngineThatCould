#include "TLETC/Core/Application.h"
#include "TLETC/Scene/Behaviour.h"
#include "TLETC/Resources/GeometryFactory.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

/**
 * Example demonstrating:
 * 1. Bit flags - only calling implemented methods
 * 2. Execution order - behaviours run in priority order
 * 3. Deferred destruction - safe mid-frame entity removal
 * 4. Event routing - entities receive input events automatically
 */

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

/**
 * EarlyBehaviour - Runs first (execution order = -100)
 * Uses clean SetEventFlags API
 */
class EarlyBehaviour : public TLETC::Behaviour 
{
public:
    EarlyBehaviour() {
        SetExecutionOrder(-100);  // Run first!
        SetActiveEvents(TLETC::Behaviour::Update);  // Clean API!
    }
    
    void OnUpdate(float dt) override {
        std::cout << "  [Early] Running first (order: " << GetExecutionOrder() << ")" << std::endl;
    }
    
    const char* GetName() const override { return "EarlyBehaviour"; }
};

/**
 * NormalBehaviour - Runs second (execution order = 0, default)
 * Uses clean SetEventFlags API
 */
class NormalBehaviour : public TLETC::Behaviour 
{
public:
    NormalBehaviour() {
        SetExecutionOrder(0);  // Default
        SetActiveEvents(TLETC::Behaviour::Update);  // Clean API!
    }
    
    void OnUpdate(float dt) override {
        std::cout << "  [Normal] Running second (order: " << GetExecutionOrder() << ")" << std::endl;
    }
    
    const char* GetName() const override { return "NormalBehaviour"; }
};

/**
 * LateBehaviour - Runs third (execution order = 100)
 * Uses clean SetEventFlags API
 */
class LateBehaviour : public TLETC::Behaviour {
public:
    LateBehaviour() {
        SetExecutionOrder(100);  // Run last!
        SetActiveEvents(TLETC::Behaviour::Update);  // Clean API!
    }
    
    void OnUpdate(float dt) override {
        std::cout << "  [Late] Running third (order: " << GetExecutionOrder() << ")" << std::endl;
    }
    
    const char* GetName() const override { return "LateBehaviour"; }
};

/**
 * KeyHandler - Only responds to key events
 * Uses clean SetEventFlags API with bitwise OR
 */
class KeyHandler : public TLETC::Behaviour {
public:
    KeyHandler() {
        SetActiveEvents(TLETC::Behaviour::KeyEvents);  // Clean API - one line!
    }
    
    void OnKeyPressed(TLETC::KeyCode key) override {
        std::cout << "  KeyHandler: Key pressed: " << static_cast<int>(key) << std::endl;
        
        if (key == TLETC::KeyCode::K) {
            std::cout << "   You pressed K! Destroying this entity in 1 second..." << std::endl;
            m_destroyTimer = 1.0f;
            ActivateEvents(TLETC::Behaviour::Update);  // Dynamically add Update event!
        }
    }
    
    void OnUpdate(float dt) override {
        if (m_destroyTimer > 0.0f) {
            m_destroyTimer -= dt;
            if (m_destroyTimer <= 0.0f) {
                std::cout << "  Destroying entity NOW (deferred destruction)!" << std::endl;
                GetEntity()->SetEnabled(false);
            }
        }
    }
    
    const char* GetName() const override { return "KeyHandler"; }
    
private:
    float m_destroyTimer = 0.0f;
};

/**
 * MouseHandler - Only responds to mouse events
 * Uses clean SetEventFlags API
 */
class MouseHandler : public TLETC::Behaviour {
public:
    MouseHandler() {
        SetActiveEvents(TLETC::Behaviour::MouseButtonEvents);  // Clean API!
    }
    
    void OnMouseButtonPressed(TLETC::MouseButton button) override {
        std::cout << "  MouseHandler: Mouse button pressed: " << static_cast<int>(button) << std::endl;
    }
    
    const char* GetName() const override { return "MouseHandler"; }
};

/**
 * Rotator - Only needs Update
 * Uses clean SetEventFlags API
 */
class Rotator : public TLETC::Behaviour {
public:
    TLETC::Vec3 axis;
    float speed;
    
    Rotator(const TLETC::Vec3& a = TLETC::Vec3(0, 1, 0), float s = 45.0f)
        : axis(a), speed(s) {
        SetActiveEvents(TLETC::Behaviour::Update);  // Clean API!
    }
    
    void OnUpdate(float dt) override {
        GetEntity()->transform.Rotate(axis, speed * dt);
    }
    
    const char* GetName() const override { return "Rotator"; }
};

/**
 * ArchitectureFixes - Demonstrates all the fixes
 */
class ArchitectureFixes : public TLETC::Locomotive {
public:
    ArchitectureFixes() : Locomotive("Architecture Fixes Demo", 1280, 720) {}
    
    void OnInit() override {
        std::cout << "========================================" << std::endl;
        std::cout << "  Architecture Fixes Demo" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        std::cout << "This demo shows:" << std::endl;
        std::cout << "  1. Bit Flags - Only implemented methods get called" << std::endl;
        std::cout << "  2. Execution Order - Behaviours run in priority order" << std::endl;
        std::cout << "  3. Deferred Destruction - Safe mid-frame removal" << std::endl;
        std::cout << "  4. Event Routing - Entities receive events automatically" << std::endl;
        std::cout << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Any Key       - KeyHandler will log it" << std::endl;
        std::cout << "  Left Click    - MouseHandler will log it" << std::endl;
        std::cout << "  K             - Destroy event test entity (deferred)" << std::endl;
        std::cout << "  ESC           - Exit" << std::endl;
        std::cout << std::endl;
        
        cubeMesh = TLETC::GeometryFactory::CreateCube(1.0f);
        sphereMesh = TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16);
        
        // Create entity with execution-ordered behaviours
        auto* orderedEntity = CreateEntity("OrderedEntity");
        orderedEntity->mesh = &cubeMesh;
        orderedEntity->transform.position = TLETC::Vec3(-3, 0, 0);
        
        // Add in WRONG order - they'll be sorted by executionOrder!
        //orderedEntity->AddBehaviour<LateBehaviour>();    // Order: 100
        //orderedEntity->AddBehaviour<EarlyBehaviour>();   // Order: -100
        //orderedEntity->AddBehaviour<NormalBehaviour>();  // Order: 0
        
        std::cout << "Created OrderedEntity with 3 behaviours" << std::endl;
        std::cout << "  - Added in wrong order: Late, Early, Normal" << std::endl;
        std::cout << "  - Will execute in correct order: Early, Normal, Late" << std::endl;
        std::cout << std::endl;
        
        // Create entity that only responds to events
        eventTestEntity = CreateEntity("EventTestEntity");
        eventTestEntity->mesh = &sphereMesh;
        eventTestEntity->transform.position = TLETC::Vec3(0, 0, 0);
        eventTestEntity->AddBehaviour<KeyHandler>();
        eventTestEntity->AddBehaviour<MouseHandler>();
        eventTestEntity->AddBehaviour<Rotator>();
        
        std::cout << "Created EventTestEntity" << std::endl;
        std::cout << "  - KeyHandler: Only responds to key events (bit flags)" << std::endl;
        std::cout << "  - MouseHandler: Only responds to mouse events (bit flags)" << std::endl;
        std::cout << "  - Rotator: Only needs Update (bit flags)" << std::endl;
        std::cout << "  Press K to test deferred destruction!" << std::endl;
        std::cout << std::endl;
        
        // Create decorative spinning cubes
        for (int i = 0; i < 5; ++i) {
            auto* spinner = CreateEntity("Spinner" + std::to_string(i));
            spinner->mesh = &cubeMesh;
            float angle = (i / 5.0f) * TLETC::TWO_PI;
            spinner->transform.position = TLETC::Vec3(
                std::cos(angle) * 5.0f,
                0,
                std::sin(angle) * 5.0f
            );
            spinner->transform.scale = TLETC::Vec3(0.5f);
            spinner->AddBehaviour<Rotator>(TLETC::Vec3(0, 1, 0), 90.0f);
        }
        
        const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
        std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string());
        std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

        // Create shaders
        auto* renderer = GetRenderDevice();
        auto vertShader = renderer->CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
        auto fragShader = renderer->CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
        shaderProgram = renderer->CreateShaderProgram(vertShader, fragShader);
        
        projection = TLETC::perspective(
            TLETC::radians(45.0f),
            GetWindow().GetAspectRatio(),
            0.1f,
            100.0f
        );
        
        std::cout << "   All systems ready!" << std::endl;
        std::cout << "   Events will be automatically routed to entities" << std::endl;
        std::cout << "   Behaviours will execute in priority order" << std::endl;
        std::cout << "   Only enabled callbacks will be called" << std::endl;
        std::cout << std::endl;
    }
    
    void OnUpdate(float dt) override {
        if (showUpdateLogs) {
            static int frameCount = 0;
            if (++frameCount % 60 == 0) {  // Log every 60 frames
                std::cout << "Frame " << frameCount << " - Execution order demo:" << std::endl;
            }
        }
    }
    
    void OnRender() override {
        auto* renderer = GetRenderDevice();
        renderer->Clear(TLETC::Vec4(0.1f, 0.1f, 0.15f, 1.0f));
        
        TLETC::Vec3 cameraPos(0, 8, 12);
        TLETC::Mat4 view = TLETC::lookAt(cameraPos, TLETC::Vec3(0, 0, 0), TLETC::Vec3(0, 1, 0));
        
        renderer->UseShader(shaderProgram);
        renderer->SetUniformMat4(shaderProgram, "u_view", view);
        renderer->SetUniformMat4(shaderProgram, "u_projection", projection);
        renderer->SetUniformVec3(shaderProgram, "u_lightPos", TLETC::Vec3(10, 10, 10));
        
        for (const auto& entity : GetEntities()) {
            if (!entity->mesh || !entity->IsEnabled()) continue;
            
            TLETC::Vec3 color;
            if (entity->name == "EventTestEntity") {
                color = TLETC::Vec3(1.0f, 0.5f, 0.0f);  // Orange
            } else if (entity->name == "OrderedEntity") {
                color = TLETC::Vec3(0.0f, 0.7f, 1.0f);  // Cyan
            } else {
                color = TLETC::Vec3(0.5f, 0.5f, 0.5f);  // Gray
            }
            
            renderer->SetUniformVec3(shaderProgram, "u_color", color);
            renderer->DrawMesh(*entity->mesh, entity->transform.GetModelMatrix());
        }
    }
    
    void OnShutdown() override {
        GetRenderDevice()->DestroyShader(shaderProgram);
    }
    
private:
    TLETC::Mesh cubeMesh, sphereMesh;
    TLETC::ShaderHandle shaderProgram;
    TLETC::Mat4 projection;
    TLETC::Entity* eventTestEntity;
    bool showUpdateLogs = false;
};

int main() {
    ArchitectureFixes app;
    
    if (!app.Initialize()) {
        return -1;
    }
    
    app.Run();
    app.Shutdown();
    
    return 0;
}
