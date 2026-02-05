#pragma once

#include <TLETC/Scene/Behaviour.h>
#include <TLETC/Resources/Mesh.h>
#include <TLETC/Rendering/Material.h>

namespace TLETC {

// Forward declarations
class RenderDevice;

/**
 * MeshRenderer - Renders a mesh with a material
 * 
 * Add this behaviour to an entity to make it visible.
 * Application automatically collects all MeshRenderers and renders them.
 * 
 * Usage:
 *   auto* entity = app.CreateEntity("Cube");
 *   auto* renderer = entity->AddBehaviour<MeshRenderer>(&cubeMesh, material);
 */
class MeshRenderer : public Behaviour 
{
public:
    MeshRenderer(const Mesh* mesh = nullptr, Material* material = nullptr);
    ~MeshRenderer();

    // Mesh
    void SetMesh(const Mesh* mesh) { mesh_ = mesh; }
    const Mesh* GetMesh() const { return mesh_; }
    
    // Material
    void SetMaterial(Material* material) { material_ = material; }
    Material* GetMaterial() const { return material_; }
    
    // Rendering (called by Application during render phase)
    void Render(RenderDevice* renderer, const Mat4& view, const Mat4& projection);
    
    const char* GetName() const override { return "MeshRenderer"; }

private:
    const Mesh* mesh_;
    Material*   material_;
};

} // namespace TLETC