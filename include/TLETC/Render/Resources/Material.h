#pragma once

#include "TLETC/Render/Shader.h"
#include "TLETC/Core/Math.h"

namespace TLETC::Render
{

struct Material
{
    Shader* shader = nullptr;
    vec4 color = vec4(1.0f);
    // TODO: textures, properties, ...
};

} // namespace TLETC::Render