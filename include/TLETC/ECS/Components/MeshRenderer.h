#pragma once

#include "TLETC/Render/Resources/Mesh.h"
#include "TLETC/Render/Resources/Material.h"
#include "TLETC/Render/Buffer.h"

namespace TLETC::ECS
{

struct MeshRenderer
{
    Render::Mesh* mesh = nullptr;
    Render::Material material;

    // GPU resources (created by RenderSystem)
    Render::VertexBuffer* vbo = nullptr;
    Render::IndexBuffer*  ibo = nullptr;

    bool castShadows    = true;
    bool receiveShadows = true;
};

} // namespace TLETC::ECS