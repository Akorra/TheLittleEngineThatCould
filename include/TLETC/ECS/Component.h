#pragma once

#include "TLETC/ECS/Entity.h"

namespace TLETC::ECS 
{

/**
 * Component 
 * Base class (attaches to entity)
 * Behaviours/Renderables/Physics etc..
 */
class Component
{
public:
    virtual ~Component() = default;
    EntityID entity = 0;
};

} // namespace TLETC::ECS