#include "TLETC/Core/Application.h"
#include "TLETC/Resources/GeometryFactory.h"
#include "TLETC/Rendering/MeshRenderer.h"
#include "TLETC/Rendering/Material.h"
#include "TLETC/Rendering/OrbitCamera.h"

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

class Spinner : public TLETC::Behaviour
{
public:
    Spinner() { SetActiveEvents(TLETC::Behaviour::EventFlag::Update); }

    void OnUpdate(float deltaTime) override 
    {
        auto e = GetEntity();
        if(!e)
        {
            std::cout << "wtf\n";
            return;
        }

        e->transform.Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), TLETC::radians(50.0f));
        e->transform.Rotate(TLETC::Vec3(1.0f, 0.0f, 0.0f), TLETC::radians(30.0f));
    }
};

class SpinnerApp : public TLETC::Locomotive 
{
public:
    TLETC::OrbitCamera*  cam = nullptr;
    TLETC::Mesh          mesh;
    TLETC::Material*     mat = nullptr;

    SpinnerApp() : Locomotive("Spin Da Cube!", 1280, 720) {}

    void OnInit() override 
    {
        std::cout << "========================================" << std::endl;
        std::cout << "  The Little Engine That Could" << std::endl;
        std::cout << "  Example: Spinning Cube" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
            
        const std::filesystem::path ProjectRoot = PROJECT_ROOT_DIR;
        std::string vertexShaderSource   = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.vert").string() );
        std::string fragmentShaderSource = LoadFileAsString( (ProjectRoot / "assets/shaders/basic.frag").string());

        // Create shader program
        auto vertexShader   = GetRenderDevice()->CreateShader(TLETC::ShaderType::Vertex, vertexShaderSource);
        auto fragmentShader = GetRenderDevice()->CreateShader(TLETC::ShaderType::Fragment, fragmentShaderSource);
        auto shaderProgram  = GetRenderDevice()->CreateShaderProgram(vertexShader, fragmentShader);
        if (!shaderProgram.IsValid()) 
            std::cerr << "Failed to create shader program!" << std::endl;
        else
            std::cout << "Shader program created successfully!" << std::endl;
        std::cout << std::endl;
        
        // Create materials using Application (automatic cleanup!)
        mat = new TLETC::Material("Red");
        mat->SetShader(shaderProgram);
        mat->SetVec3("u_color",    TLETC::Vec3(1.0f, 0.5f, 0.2f));
        mat->SetVec3("u_lightPos", TLETC::Vec3(2.0f, 2.0f, 2.0f));
        mat->SetVec3("u_viewPos",  TLETC::Vec3(0, 2, 0));
        
        std::cout << "\nCreated 1 materials (Application owns them)" << std::endl;
        
        // Create cube
        mesh = TLETC::GeometryFactory::CreateCube(1.0f);
        
        auto* cube = CreateEntity("TexturedCube");
        cube->transform.position_ = TLETC::Vec3(0, 2, 0);
        cube->AddBehaviour<Spinner>();
        cube->AddBehaviour<TLETC::MeshRenderer>(&mesh, mat);

        // Create orbit camera
        auto* camEntity = CreateEntity("Camera");
        cam = camEntity->AddBehaviour<TLETC::OrbitCamera>();
        cam->SetTarget(cube->transform.position_);
        cam->SetDistance(10.0f);
        cam->SetPerspective(45.0f, 0.1f, 100.0f);
        SetCamera(cam);
        
        std::cout << "\nAll resources created! Application will clean up automatically.\n" << std::endl;
    }

    void Destroy()
    {
        delete mat;
    }
};

int main() 
{
    SpinnerApp app;
    if(!app.Initialize())
        return -1;
    app.Run();
    app.Shutdown();
    app.Destroy();
    return 0;
}