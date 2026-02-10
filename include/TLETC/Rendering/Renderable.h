#pragma once

#include "TLETC/Scene/Component.h"
#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/Material.h"

namespace TLETC 
{
class Renderable : public Component
{
public:
    explicit Renderable(Entity* owner);

    void SetMesh(MeshHandle mesh)             { mesh_ = mesh; }
    void SetMaterial(MaterialHandle material) { material_ = material; }
    void SetLayer(uint32 mask)                { layerMask_ = mask; }

    MeshHandle     GetMesh()     const { return mesh_; }
    MaterialHandle GetMaterial() const { return material_; }
    uint32         GetLayer()    const { return layerMask_; }

    virtual void OnStart()   override;
    virtual void OnDestroy() override;

private:
    MeshHandle     mesh_;
    MaterialHandle material_;
    uint32         layerMask_ = 1;
};

} // namespace TLETC