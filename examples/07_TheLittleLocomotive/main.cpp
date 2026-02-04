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

// Little Train Car behaviour
class TrainCar : public TLETC::Behaviour 
{
public:
    float speed = 2.0f;
    float pathProgress = 0.0f;

    TrainCar() { SetActiveEvents(EventFlag::Update | EventFlag::KeyEvents); }
    
    void OnUpdate(float deltaTime) override 
    {
        // Chug along the circular track
        pathProgress += speed * deltaTime;
        
        float radius = 5.0f;
        GetEntity()->transform.position.x = std::cos(pathProgress) * radius;
        GetEntity()->transform.position.z = std::sin(pathProgress) * radius;
        
        // Face forward along the track
        float angle = pathProgress + TLETC::HALF_PI;
        GetEntity()->transform.rotation = TLETC::angleAxis(angle, TLETC::Vec3(0, 1, 0));
    }
    
    void OnKeyPressed(TLETC::KeyCode key) override 
    {
        if (key == TLETC::KeyCode::Up) 
        {
            speed += 0.5f;
            std::cout << "Faster! Speed: " << speed << std::endl;
        }
        if (key == TLETC::KeyCode::Down) 
        {
            speed = std::max(0.0f, speed - 0.5f);
            std::cout << "Slower! Speed: " << speed << std::endl;
        }
    }
    
    const char* GetName() const override { return "TrainCar"; }
};

// Smokestack effect
class Smokestack : public TLETC::Behaviour {
public:
    float time = 0.0f;

    Smokestack() { SetActiveEvents(EventFlag::Update); }

    void OnUpdate(float deltaTime) override 
    {
        time += deltaTime * 3.0f;
        // Bob up and down like smoke puffing
        GetEntity()->transform.position.y = 0.5f + std::sin(time) * 0.2f;
    }
    
    const char* GetName() const override { return "Smokestack"; }
};

/**
 * TheLittleLocomotiveThatCould
 * 
 * Using the Locomotive alias for Application!
 * "I think I can! I think I can!"
 */
class TheLittleLocomotiveThatCould : public TLETC::Locomotive {
public:
    TheLittleLocomotiveThatCould() 
        : Locomotive("The Little Locomotive That Could!", 1280, 720) {}
    
    void OnInit() override {
        std::cout << "========================================" << std::endl;
        std::cout << "  The Little Locomotive That Could!" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        std::cout << "\"I think I can! I think I can!\"" << std::endl;
        std::cout << std::endl;
        std::cout << "This example uses the 'Locomotive' alias" << std::endl;
        std::cout << "for Application - same class, fun name!" << std::endl;
        std::cout << std::endl;
        std::cout << "Watch the little train chug around the track!" << std::endl;
        std::cout << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  UP    - Go faster!" << std::endl;
        std::cout << "  DOWN  - Go slower" << std::endl;
        std::cout << "  ESC   - Stop the train" << std::endl;
        std::cout << std::endl;
        
        // Create track (circle of cubes)
        cubeMesh = TLETC::GeometryFactory::CreateCube(0.3f);
        sphereMesh = TLETC::GeometryFactory::CreateSphere(0.3f, 16, 12);
        cylinderMesh = TLETC::GeometryFactory::CreateCylinder(0.2f, 0.6f, 16);
        
        // Place track markers
        for (int i = 0; i < 20; ++i) 
        {
            float angle = (i / 20.0f) * TLETC::TWO_PI;
            float radius = 5.0f;
            
            auto* marker = CreateEntity("Track" + std::to_string(i));
            marker->mesh = &cubeMesh;
            marker->transform.position = TLETC::Vec3(std::cos(angle) * radius, -0.5f, std::sin(angle) * radius);
            marker->transform.scale = TLETC::Vec3(0.5f, 0.1f, 0.5f);
        }
        
        // Create the little locomotive (engine)
        engine = CreateEntity("Locomotive");
        engine->mesh = &cubeMesh;
        engine->transform.scale = TLETC::Vec3(0.8f, 0.6f, 1.2f);
        engine->AddBehaviour<TrainCar>();
        
        // Add smokestack
        smokestack = CreateEntity("Smokestack");
        smokestack->mesh = &cylinderMesh;
        smokestack->transform.scale = TLETC::Vec3(0.5f, 1.0f, 0.5f);
        smokestack->AddBehaviour<Smokestack>();
        
        // Add some train cars following
        for (int i = 0; i < 3; ++i) 
        {
            auto* car = CreateEntity("Car" + std::to_string(i + 1));
            car->mesh = &cubeMesh;
            car->transform.scale = TLETC::Vec3(0.7f, 0.5f, 1.0f);
            auto* carBehaviour = car->AddBehaviour<TrainCar>();
            carBehaviour->pathProgress = -(i + 1) * 1.0f; // Stagger behind
        }
        
        const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
        std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string());
        std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

        // Create shaders
        auto* renderer = GetRenderDevice();
        auto vertShader = renderer->CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
        auto fragShader = renderer->CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
        shaderProgram = renderer->CreateShaderProgram(vertShader, fragShader);
        
        // Setup camera
        projection = TLETC::perspective(
            TLETC::radians(45.0f),
            GetWindow().GetAspectRatio(),
            0.1f,
            100.0f
        );
        
        std::cout << "🚂 Choo choo! The little locomotive is ready!" << std::endl;
        std::cout << std::endl;
    }
    
    void OnUpdate(float deltaTime) override 
    {
        // Update smokestack position to follow engine
        if (engine && smokestack) 
        {
            smokestack->transform.position.x = engine->transform.position.x;
            smokestack->transform.position.z = engine->transform.position.z;
        }
    }
    
    void OnRender() override 
    {
        auto* renderer = GetRenderDevice();
        
        // Clear with a nice sky blue
        renderer->Clear(TLETC::Vec4(0.5f, 0.7f, 1.0f, 1.0f));
        
        // Setup camera looking down at the track
        TLETC::Vec3 cameraPos(0, 8, 12);
        TLETC::Mat4 view = TLETC::lookAt(
            cameraPos,
            TLETC::Vec3(0, 0, 0),
            TLETC::Vec3(0, 1, 0)
        );
        
        renderer->UseShader(shaderProgram);
        renderer->SetUniformMat4(shaderProgram, "u_view", view);
        renderer->SetUniformMat4(shaderProgram, "u_projection", projection);
        renderer->SetUniformVec3(shaderProgram, "u_lightPos", TLETC::Vec3(10, 10, 10));
        
        // Draw all entities with colors
        for (const auto& entity : GetEntities()) {
            if (!entity->mesh) continue;
            
            TLETC::Vec3 color;
            
            if (entity->name == "Locomotive") {
                color = TLETC::Vec3(0.8f, 0.1f, 0.1f); // Red engine
            } else if (entity->name == "Smokestack") {
                color = TLETC::Vec3(0.3f, 0.3f, 0.3f); // Dark gray smoke
            } else if (entity->name.find("Car") != std::string::npos) {
                color = TLETC::Vec3(0.2f, 0.4f, 0.8f); // Blue cars
            } else if (entity->name.find("Track") != std::string::npos) {
                color = TLETC::Vec3(0.4f, 0.3f, 0.2f); // Brown track
            } else {
                color = TLETC::Vec3(0.7f, 0.7f, 0.7f); // Gray default
            }
            
            renderer->SetUniformVec3(shaderProgram, "u_color", color);
            renderer->DrawMesh(*entity->mesh, entity->transform.GetModelMatrix());
        }
    }
    
    void OnShutdown() override 
    {
        GetRenderDevice()->DestroyShader(shaderProgram);
        std::cout << "🚂 \"I thought I could! I thought I could!\"" << std::endl;
        std::cout << "The little locomotive has stopped." << std::endl;
    }
    
private:
    TLETC::Mesh cubeMesh, sphereMesh, cylinderMesh;
    TLETC::Entity* engine;
    TLETC::Entity* smokestack;
    TLETC::ShaderHandle shaderProgram;
    TLETC::Mat4 projection;
};

int main() 
{
    // Using the Locomotive alias!
    TheLittleLocomotiveThatCould locomotive;
    
    if (!locomotive.Initialize())
        return -1;
    
    locomotive.Run();
    locomotive.Shutdown();
    
    return 0;
}
