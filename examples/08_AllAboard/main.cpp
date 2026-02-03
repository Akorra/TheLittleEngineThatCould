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

/**
 * All Aboard! 🚂
 * 
 * This example uses ONLY railroad-themed aliases!
 * Every class name is railroad terminology.
 */

 /**
 * PlayerCargo - What the player car carries
 * Using Cargo alias for Behaviour
 */
class PlayerCargo : public TLETC::Cargo {
public:
    float moveSpeed = 3.0f;
    
    void OnUpdate(float deltaTime) override {
        // Get the throttle (input controls)
        auto* throttle = GetInput();
        if (!throttle) return;
        
        TLETC::Vec3& pos = GetEntity()->transform.position;
        
        if (throttle->IsKeyPressed(TLETC::KeyCode::W)) pos.z -= moveSpeed * deltaTime;
        if (throttle->IsKeyPressed(TLETC::KeyCode::S)) pos.z += moveSpeed * deltaTime;
        if (throttle->IsKeyPressed(TLETC::KeyCode::A)) pos.x -= moveSpeed * deltaTime;
        if (throttle->IsKeyPressed(TLETC::KeyCode::D)) pos.x += moveSpeed * deltaTime;
    }
    
    const char* GetName() const override { return "PlayerCargo"; }
};

/**
 * RotatingCargo - Makes cars spin
 * Using Cargo alias for Behaviour
 */
class RotatingCargo : public TLETC::Cargo {
public:
    TLETC::Vec3 axis;
    float speed;
    
    RotatingCargo(const TLETC::Vec3& a = TLETC::Vec3(0, 1, 0), float s = 45.0f)
        : axis(a), speed(s) {}
    
    void OnUpdate(float deltaTime) override {
        GetEntity()->transform.Rotate(axis, speed * deltaTime);
    }
    
    const char* GetName() const override { return "RotatingCargo"; }
};

/**
 * ColorChangeCargo - Changes color on click
 * Using Cargo alias for Behaviour
 */
class ColorChangeCargo : public TLETC::Cargo {
public:
    TLETC::Vec3 color = TLETC::Vec3(1, 0, 0);
    int colorIndex = 0;
    
    void OnMouseButtonPressed(TLETC::MouseButton button) override {
        if (button == TLETC::MouseButton::Left) {
            colorIndex = (colorIndex + 1) % 3;
            switch (colorIndex) {
                case 0: color = TLETC::Vec3(1, 0, 0); break; // Red
                case 1: color = TLETC::Vec3(0, 1, 0); break; // Green
                case 2: color = TLETC::Vec3(0, 0, 1); break; // Blue
            }
            std::cout << "Car changed color!" << std::endl;
        }
    }
    
    const char* GetName() const override { return "ColorChangeCargo"; }
};

/**
 * AllAboardLocomotive
 * 
 * Using ALL railroad aliases:
 * - Locomotive (Application)
 * - Station (Window)
 * - Firebox (RenderDevice)
 * - Throttle (Input)
 * - Telegraph (EventDispatcher)
 * - Car (Entity)
 * - Cargo (Behaviour)
 */
class AllAboardLocomotive : public TLETC::Locomotive {
public:
    AllAboardLocomotive() 
        : Locomotive("All Aboard! Full Railroad Metaphor", 1280, 720) {}
    
    void OnInit() override {
        std::cout << "========================================" << std::endl;
        std::cout << "  ALL ABOARD!" << std::endl;
        std::cout << "  Full Railroad Metaphor Example" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        std::cout << "This example uses ONLY railroad terminology!" << std::endl;
        std::cout << std::endl;
        std::cout << "Railroad Glossary:" << std::endl;
        std::cout << "  Locomotive  = Application (main engine)" << std::endl;
        std::cout << "  Station     = Window (where you observe)" << std::endl;
        std::cout << "  Firebox     = RenderDevice (generates power)" << std::endl;
        std::cout << "  Throttle    = Input (controls)" << std::endl;
        std::cout << "  Telegraph   = EventDispatcher (signals)" << std::endl;
        std::cout << "  Wagon       = Entity (game object)" << std::endl;
        std::cout << "  Cargo       = Behaviour (component)" << std::endl;
        std::cout << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD       - Move the player car" << std::endl;
        std::cout << "  Left Click - Change car colors" << std::endl;
        std::cout << "  ESC        - Stop the locomotive" << std::endl;
        std::cout << std::endl;
        
        // Get the firebox (render device) using railroad alias!
        firebox = GetRenderDevice();  // Could also call GetFirebox() if we add that method
        
        // Get the throttle (input) using railroad alias!
        throttle = &GetInput();  // Could also call GetThrottle() if we add that method
        
        // Get the telegraph (event dispatcher) using railroad alias!
        telegraph = &GetEventDispatcher();  // Could also call GetTelegraph()
        
        // Get the station (window) using railroad alias!
        station = &GetWindow();  // Could also call GetStation()
        
        std::cout << "Station created: " << station->GetWidth() << "x" << station->GetHeight() << std::endl;
        std::cout << "Firebox initialized: " << firebox->GetRendererName() << std::endl;
        std::cout << "Throttle ready for input" << std::endl;
        std::cout << "Telegraph ready for signals" << std::endl;
        std::cout << std::endl;
        
        // Create meshes
        cubeMesh = TLETC::GeometryFactory::CreateCube(1.0f);
        sphereMesh = TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16);
        
        // Create the player car (entity) with cargo (behaviours)!
        TLETC::Wagon* playerCar = CreateEntity("PlayerCar");  // Using Car alias!
        playerCar->mesh = &cubeMesh;
        playerCar->transform.position = TLETC::Vec3(0, 0, 0);
        playerCar->AddBehaviour<PlayerCargo>();  // Could use AddCargo()!
        playerColorCargo = playerCar->AddBehaviour<ColorChangeCargo>();
        playerColorCargo->color = TLETC::Vec3(0.2f, 0.4f, 1.0f); // Blue
        
        std::cout << "Player car created with cargo:" << std::endl;
        std::cout << "   - PlayerCargo (movement)" << std::endl;
        std::cout << "   - ColorChangeCargo (color cycling)" << std::endl;
        std::cout << std::endl;
        
        // Create spinning cars (entities) with rotating cargo (behaviours)
        for (int i = 0; i < 5; ++i) {
            TLETC::Wagon* spinningCar = CreateEntity("SpinningCar" + std::to_string(i));  // Car alias!
            spinningCar->mesh = &sphereMesh;
            
            float angle = (i / 5.0f) * TLETC::TWO_PI;
            spinningCar->transform.position = TLETC::Vec3(
                std::cos(angle) * 5.0f,
                0,
                std::sin(angle) * 5.0f
            );
            
            spinningCar->AddBehaviour<RotatingCargo>(TLETC::Vec3(0, 1, 0), 90.0f);  // Cargo alias!
            auto* colorCargo = spinningCar->AddBehaviour<ColorChangeCargo>();  // Cargo alias!
            colorCargo->color = TLETC::Vec3(
                0.5f + 0.5f * std::sin(i),
                0.5f + 0.5f * std::cos(i),
                0.5f
            );
        }
        
        std::cout << "5 spinning cars created with rotating cargo" << std::endl;
        std::cout << std::endl;

        const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
        std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string() );
        std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());
        
        // Create shaders using the firebox
        auto vertShader = firebox->CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
        auto fragShader = firebox->CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
        shaderProgram = firebox->CreateShaderProgram(vertShader, fragShader);
        
        // Setup projection
        projection = TLETC::perspective(
            TLETC::radians(45.0f),
            station->GetAspectRatio(),  // Using station (Window) alias!
            0.1f,
            100.0f
        );
        
        // Subscribe to telegraph signals (events)
        telegraph->Subscribe<TLETC::KeyPressedEvent>([this](TLETC::KeyPressedEvent& e) {
            if (e.key == TLETC::KeyCode::T) {
                std::cout << "Telegraph received signal: T key pressed!" << std::endl;
            }
        });
        
        std::cout <<  "All systems ready! The locomotive is departing!" << std::endl;
        std::cout << "   \"I think I can! I think I can!\"" << std::endl;
        std::cout << std::endl;
    }
    
    void OnRender() override {
        // Use the firebox to render (RenderDevice alias)
        firebox->Clear(TLETC::Vec4(0.1f, 0.15f, 0.2f, 1.0f));
        
        // Setup camera
        TLETC::Vec3 cameraPos(0, 8, 12);
        TLETC::Mat4 view = TLETC::lookAt(
            cameraPos,
            TLETC::Vec3(0, 0, 0),
            TLETC::Vec3(0, 1, 0)
        );
        
        firebox->UseShader(shaderProgram);
        firebox->SetUniformMat4(shaderProgram, "u_view", view);
        firebox->SetUniformMat4(shaderProgram, "u_projection", projection);
        firebox->SetUniformVec3(shaderProgram, "u_lightPos", TLETC::Vec3(10, 10, 10));
        
        // Draw all cars (entities)
        for (const auto& car : GetEntities()) {  // Cars!
            if (!car->mesh) continue;
            
            TLETC::Vec3 color;
            
            if (car->name == "PlayerCar") {
                color = playerColorCargo->color;
            } else {
                auto* colorCargo = car->GetBehaviour<ColorChangeCargo>();
                color = colorCargo ? colorCargo->color : TLETC::Vec3(0.7f);
            }
            
            firebox->SetUniformVec3(shaderProgram, "u_color", color);
            firebox->DrawMesh(*car->mesh, car->transform.GetModelMatrix());
        }
    }
    
    void OnShutdown() override {
        firebox->DestroyShader(shaderProgram);
        
        std::cout << std::endl;
        std::cout << "\"I thought I could! I thought I could!\"" << std::endl;
        std::cout << "The locomotive has arrived at the station." << std::endl;
        std::cout << std::endl;
        std::cout << "Railroad Metaphor Complete!" << std::endl;
    }
    
private:
    // Using traditional names for members (they're private anyway)
    TLETC::Mesh cubeMesh, sphereMesh;
    TLETC::ShaderHandle shaderProgram;
    TLETC::Mat4 projection;
    ColorChangeCargo* playerColorCargo;
    
    // Could use aliases here too!
    TLETC::Firebox* firebox;        // RenderDevice
    TLETC::Throttle* throttle;      // Input
    TLETC::Telegraph* telegraph;    // EventDispatcher
    TLETC::Station* station;        // Window
};

int main() {
    // Using the Locomotive alias (Application)!
    AllAboardLocomotive locomotive;
    
    if (!locomotive.Initialize()) {
        return -1;
    }
    
    locomotive.Run();
    locomotive.Shutdown();
    
    return 0;
}