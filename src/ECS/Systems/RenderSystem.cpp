#include "TLETC/ECS/Systems/RenderSystem.h"
#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Components/Camera.h"
#include "TLETC/ECS/Components/MeshRenderer.h"
#include "TLETC/ECS/Scene.h"
#include "TLETC/Core/Log.h"

namespace TLETC::ECS
{

RenderSystem::RenderSystem(Render::RenderDevice* device) : device_(device)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Startup(Scene& scene)
{
    TLETC_INFO("RenderSystem started");
}

void RenderSystem::Shutdown(Scene& scene)
{
    // Clean up GPU resources
    scene.View<MeshRenderer>([&](Entity e, MeshRenderer& renderer) {
        if (renderer.positionVBO) device_->DestroyVertexBuffer(renderer.positionVBO);
        if (renderer.normalVBO)   device_->DestroyVertexBuffer(renderer.normalVBO);  
        if (renderer.uvVBO)       device_->DestroyVertexBuffer(renderer.uvVBO);
        if (renderer.colorVBO)    device_->DestroyVertexBuffer(renderer.colorVBO);
        if (renderer.ibo)         device_->DestroyIndexBuffer(renderer.ibo);
            
        renderer.ibo         = nullptr;
        renderer.positionVBO = nullptr;
        renderer.normalVBO   = nullptr;
        renderer.uvVBO       = nullptr;
        renderer.colorVBO    = nullptr;
    });

    TLETC_INFO("RenderSystem shutdown");
}

void RenderSystem::Render(Scene& scene, float alpha)
{
    // Find primary camera
    Camera* primaryCamera = nullptr;
    Transform* cameraTransform = nullptr;

    scene.View<Camera, Transform>([&](Entity e, Camera& cam, Transform& t) {
        if (cam.isPrimary)
        {
            primaryCamera = &cam;
            cameraTransform = &t;
        }
    });

    if (!primaryCamera || !cameraTransform)
        return;  // No camera to render from

    // Calculate view and projection matrices
    mat4 view = inverse(cameraTransform->GetWorldMatrix(scene));
    mat4 proj = primaryCamera->GetProjectionMatrix(16.0f / 9.0f);  // TODO: Get aspect from window
    mat4 viewProj = proj * view;

    // Render all mesh renderers
    scene.View<Transform, MeshRenderer>([&](Entity e, Transform& t, MeshRenderer& renderer) {
        RenderMesh(scene, e, renderer, viewProj);
    });
}

void RenderSystem::UploadMesh(MeshRenderer& renderer)
{
    if (!renderer.mesh)
        return;

    auto& positions = renderer.mesh->GetVertexPositions();
    auto& normals   = renderer.mesh->GetVertexNormals();
    auto& uvs       = renderer.mesh->GetVertexUVs();
    auto& colors    = renderer.mesh->GetVertexColors();
    auto& indices   = renderer.mesh->GetIndices();

    // Create attribute buffers (SoA)
    renderer.positionVBO = device_->CreateVertexBuffer(
        positions.data(),
        positions.size() * sizeof(vec3),
        Render::BufferUsage::Static
    );

    renderer.normalVBO = device_->CreateVertexBuffer(
        normals.data(),
        normals.size() * sizeof(vec3),
        Render::BufferUsage::Static
    );

    renderer.uvVBO = device_->CreateVertexBuffer(
        uvs.data(),
        uvs.size() * sizeof(vec2),
        Render::BufferUsage::Static
    );

    renderer.colorVBO = device_->CreateVertexBuffer(
        colors.data(),
        colors.size() * sizeof(vec4),
        Render::BufferUsage::Static
    );

    // Layout: each buffer contains only one attribute
    renderer.positionVBO->SetLayout({
        { Render::VertexAttributeType::Float3, 0 }
    });

    renderer.normalVBO->SetLayout({
        { Render::VertexAttributeType::Float3, 0 }
    });

    renderer.uvVBO->SetLayout({
        { Render::VertexAttributeType::Float2, 0 }
    });

    renderer.colorVBO->SetLayout({
        { Render::VertexAttributeType::Float4, 0 }
    });

    // Create index buffer
    renderer.ibo = device_->CreateIndexBuffer(
        indices.data(),
        indices.size(),
        Render::BufferUsage::Static
    );
}

void RenderSystem::RenderMesh(Scene& scene, Entity entity, MeshRenderer& renderer, const mat4& viewProj)
{
    // Upload mesh if not already uploaded
    if (!renderer.positionVBO || !renderer.ibo)
        UploadMesh(renderer);

    if (!renderer.material.shader)
        return;

    Transform* transform = scene.GetComponent<Transform>(entity);
    if (!transform)
        return;

    // Bind shader
    device_->BindShader(renderer.material.shader);

    // Set uniforms
    mat4 mvp = viewProj * transform->GetWorldMatrix(scene);
    renderer.material.shader->SetMat4("u_MVP", mvp);
    renderer.material.shader->SetMat4("u_Model", transform->GetWorldMatrix(scene));
    renderer.material.shader->SetVec4("u_Color", renderer.material.color);

    // Bind buffers
    device_->BindVertexBuffer(renderer.positionVBO, 0);
    device_->BindVertexBuffer(renderer.normalVBO,   1);
    device_->BindVertexBuffer(renderer.uvVBO,       2);
    device_->BindVertexBuffer(renderer.colorVBO,    3);
    device_->BindIndexBuffer(renderer.ibo);

    // Draw
    device_->DrawIndexed(Render::PrimitiveType::Triangles, renderer.ibo->GetCount());
}

} // namespace TLETC::ECS