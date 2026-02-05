#include "TLETC/Rendering/MeshRenderer.h"
#include "TLETC/Rendering/RenderDevice.h"
#include "TLETC/Scene/Entity.h"

namespace TLETC {

MeshRenderer::MeshRenderer(const Mesh* mesh, Material* material)
    : mesh_(mesh)
    , material_(material)
{
    // MeshRenderer doesn't need any event callbacks
    // Application handles rendering directly
    SetActiveEvents(EventFlag::None);
}

MeshRenderer::~MeshRenderer() 
{
}

void MeshRenderer::Render(RenderDevice* renderer, const Mat4& view, const Mat4& projection) 
{
    if (!mesh_ || !material_) return;
    if (!IsEnabled()) return;
    
    // Get entity transform
    Mat4 model = GetEntity()->transform.GetModelMatrix();
    
    // Bind material (sets shader + all properties + render state)
    material_->Bind(renderer);
    
    // Set transform matrices (common to all shaders)
    ShaderHandle shader = material_->GetShader();
    renderer->SetUniformMat4(shader, "u_model", model);
    renderer->SetUniformMat4(shader, "u_view", view);
    renderer->SetUniformMat4(shader, "u_projection", projection);
    
    // Optional: Set normal matrix for lighting
    Mat3 normalMatrix = Mat3(transpose(inverse(model)));
    renderer->SetUniformMat3(shader, "u_normalMatrix", normalMatrix);

    //TODO: Set tess patches.
    
    // Draw the mesh
    renderer->DrawMesh(*mesh_, model);
}

} // namespace TLETC