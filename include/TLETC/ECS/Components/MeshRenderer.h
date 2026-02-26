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
    // SoA: one buffer per attribute
    Render::VertexBuffer* positionVBO = nullptr;
    Render::VertexBuffer* normalVBO   = nullptr;
    Render::VertexBuffer* uvVBO       = nullptr;
    Render::VertexBuffer* colorVBO    = nullptr;

    Render::IndexBuffer*  ibo = nullptr;

    bool castShadows    = true;
    bool receiveShadows = true;
};

} // namespace TLETC::ECS