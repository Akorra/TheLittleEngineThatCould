#pragma once

#include "TLETC/Core/Math.h"
#include "TLETC/ECS/Entity.h"

namespace TLETC::ECS
{
struct Transform
{
    // Local
    vec3 position {0};
    quat rotation {};
    vec3 scale    {1};

    // Render interpolates between prev and current world matrices:
    // World (current fixed step result)
    mat4 worldMatrix;  //< Derived
    // World (previous fixed step result)
    mat4 prevWorldMatrix {}; //< Update only during fixed update

    // Hierarchy
    Entity parent{};
    std::vector<Entity> children;

    // State
    bool dirty = true;
};
}