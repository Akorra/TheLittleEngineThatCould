// Example 12: Automatic Resource Management
// Demonstrates Application-managed materials and textures with automatic cleanup

#include "TLETC/Core/Application.h"
#include "TLETC/Resources/GeometryFactory.h"
#include "TLETC/Rendering/MeshRenderer.h"
#include "TLETC/Rendering/OrbitCamera.h"
#include "TLETC/Rendering/Material.h"
#include "TLETC/Rendering/Texture.h"

#include <iostream>

using namespace TLETC;

class AutoResourceExample : public Application 
{
public:
    // Just pointers - Application owns them
    Material* redMaterial;
    Material* greenMaterial;
    Material* blueMaterial;
    Material* texturedMaterial;
    Texture*  checkerTexture;
    Mesh      cubeMesh;

public:
    AutoResourceExample() : Application("Auto Resource Management", 1280, 720) {}
    
protected:
    void OnInit() override {
        std::cout << "\n=== Automatic Resource Management Demo ===" << std::endl;
        std::cout << "Application will automatically clean up all resources!" << std::endl;
        std::cout << "\nControls:" << std::endl;
        std::cout << "  Left Mouse + Drag: Rotate camera" << std::endl;
        std::cout << "  Right Mouse + Drag: Pan camera" << std::endl;
        std::cout << "  Mouse Wheel: Zoom" << std::endl;
        std::cout << "  ESC: Quit (watch cleanup messages!)" << std::endl;
        
        // Create shader
        auto shader = CreateSimpleShader();
        
        // Create materials using Application (automatic cleanup!)
        redMaterial = new Material("Red");
        redMaterial->SetShader(shader);
        redMaterial->SetVec3("u_color", Vec3(1.0f, 0.0f, 0.0f));
        
        greenMaterial = new Material("Green");
        greenMaterial->SetShader(shader);
        greenMaterial->SetVec3("u_color", Vec3(0.0f, 1.0f, 0.0f));
        
        blueMaterial = new Material("Blue");
        blueMaterial->SetShader(shader);
        blueMaterial->SetVec3("u_color", Vec3(0.0f, 0.0f, 1.0f));
        
        std::cout << "\nCreated 3 materials (Application owns them)" << std::endl;
        
        // Create procedural texture using Application (automatic cleanup!)
        checkerTexture = new Texture();
        CreateCheckerboardTexture(checkerTexture);
        
        texturedMaterial = new Material("Checkerboard");
        texturedMaterial->SetShader(CreateTexturedShader());
        texturedMaterial->SetTexture("u_texture", checkerTexture);
        
        std::cout << "Created 1 texture (Application owns it)" << std::endl;
        
        // Create cubes with different materials
        Mesh cubeMesh = GeometryFactory::CreateCube(1.0f);
        
        auto* redCube = CreateEntity("RedCube");
        redCube->transform.position_ = Vec3(-3, 0, 0);
        redCube->AddBehaviour<MeshRenderer>(&cubeMesh, redMaterial);
        
        auto* greenCube = CreateEntity("GreenCube");
        greenCube->transform.position_ = Vec3(0, 0, 0);
        greenCube->AddBehaviour<MeshRenderer>(&cubeMesh, greenMaterial);
        
        auto* blueCube = CreateEntity("BlueCube");
        blueCube->transform.position_ = Vec3(3, 0, 0);
        blueCube->AddBehaviour<MeshRenderer>(&cubeMesh, blueMaterial);
        
        auto* texturedCube = CreateEntity("TexturedCube");
        texturedCube->transform.position_ = Vec3(0, 2, 0);
        texturedCube->AddBehaviour<MeshRenderer>(&cubeMesh, texturedMaterial);
        
        // Create orbit camera
        auto* camEntity = CreateEntity("Camera");
        auto* orbitCam = camEntity->AddBehaviour<OrbitCamera>();
        orbitCam->SetTarget(Vec3(0, 0, 0));
        orbitCam->SetDistance(10.0f);
        orbitCam->SetPerspective(45.0f, 0.1f, 100.0f);
        SetCamera(orbitCam);
        
        std::cout << "\nAll resources created! Application will clean up automatically.\n" << std::endl;
    }
    
    // NO OnShutdown needed! Application cleans up automatically! ✅
    
private:
    void CreateCheckerboardTexture(Texture* texture) {
        const int size = 256;
        unsigned char* pixels = new unsigned char[size * size * 4];
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                bool white = ((x / 32) + (y / 32)) % 2 == 0;
                int idx = (y * size + x) * 4;
                pixels[idx + 0] = white ? 255 : 64;
                pixels[idx + 1] = white ? 255 : 64;
                pixels[idx + 2] = white ? 255 : 64;
                pixels[idx + 3] = 255;
            }
        }
        
        texture->CreateFromMemory(size, size, TextureFormat::RGBA, pixels, GetRenderDevice());
        delete[] pixels;
    }
    
    ShaderHandle CreateSimpleShader() {
        const char* vertCode = R"(
            #version 330 core
            layout (location = 0) in vec3 a_position;
            layout (location = 1) in vec3 a_normal;
            
            out vec3 v_normal;
            out vec3 v_fragPos;
            
            uniform mat4 u_model;
            uniform mat4 u_view;
            uniform mat4 u_projection;
            uniform mat3 u_normalMatrix;
            
            void main() {
                v_fragPos = vec3(u_model * vec4(a_position, 1.0));
                v_normal = u_normalMatrix * a_normal;
                gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
            }
        )";
        
        const char* fragCode = R"(
            #version 330 core
            out vec4 FragColor;
            
            in vec3 v_normal;
            in vec3 v_fragPos;
            
            uniform vec3 u_color;
            
            void main() {
                // Simple lighting
                vec3 lightPos = vec3(5.0, 5.0, 5.0);
                vec3 lightDir = normalize(lightPos - v_fragPos);
                float diff = max(dot(normalize(v_normal), lightDir), 0.0);
                
                // Ambient + diffuse
                vec3 ambient = 0.3 * u_color;
                vec3 diffuse = 0.7 * diff * u_color;
                vec3 result = ambient + diffuse;
                
                FragColor = vec4(result, 1.0);
            }
        )";
        
        auto vert = GetRenderDevice()->CreateShader(ShaderType::Vertex, vertCode);
        auto frag = GetRenderDevice()->CreateShader(ShaderType::Fragment, fragCode);
        return GetRenderDevice()->CreateShaderProgram(vert, frag);
    }
    
    ShaderHandle CreateTexturedShader() {
        const char* vertCode = R"(
            #version 330 core
            layout (location = 0) in vec3 a_position;
            layout (location = 1) in vec3 a_normal;
            layout (location = 2) in vec2 a_texCoord;
            
            out vec2 v_texCoord;
            out vec3 v_normal;
            out vec3 v_fragPos;
            
            uniform mat4 u_model;
            uniform mat4 u_view;
            uniform mat4 u_projection;
            uniform mat3 u_normalMatrix;
            
            void main() {
                v_fragPos = vec3(u_model * vec4(a_position, 1.0));
                v_normal = u_normalMatrix * a_normal;
                v_texCoord = a_texCoord;
                gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
            }
        )";
        
        const char* fragCode = R"(
            #version 330 core
            out vec4 FragColor;
            
            in vec2 v_texCoord;
            in vec3 v_normal;
            in vec3 v_fragPos;
            
            uniform sampler2D u_texture;
            
            void main() {
                // Sample texture
                vec4 texColor = texture(u_texture, v_texCoord);
                
                // Simple lighting
                vec3 lightPos = vec3(5.0, 5.0, 5.0);
                vec3 lightDir = normalize(lightPos - v_fragPos);
                float diff = max(dot(normalize(v_normal), lightDir), 0.0);
                
                // Apply lighting to texture
                vec3 ambient = 0.3 * texColor.rgb;
                vec3 diffuse = 0.7 * diff * texColor.rgb;
                vec3 result = ambient + diffuse;
                
                FragColor = vec4(result, 1.0);
            }
        )";
        
        auto vert = GetRenderDevice()->CreateShader(ShaderType::Vertex, vertCode);
        auto frag = GetRenderDevice()->CreateShader(ShaderType::Fragment, fragCode);
        return GetRenderDevice()->CreateShaderProgram(vert, frag);
    }

public:
    void Destroy()
    {
        checkerTexture->Destroy(GetRenderDevice());
        delete checkerTexture;
        delete redMaterial;
        delete greenMaterial;
        delete blueMaterial;
        delete texturedMaterial;
        delete checkerTexture;
    }
};

int main() 
{
    AutoResourceExample app;
    if(!app.Initialize())
        return -1;
    app.Run();
    app.Shutdown();
    app.Destroy();
    return 0;
}