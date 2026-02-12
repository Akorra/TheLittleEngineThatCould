#pragma once

#include "TLETC/Core/Types.h"

namespace TLETC::ECS 
{

using EntityID = uint32;

/**
 * Entity 
 * Lightwaight ID
 * Registered in Scene
 */
struct Entity
{
    EntityID id = 0;
    uint32   generation = 0;

    bool operator==(const Entity& other) const 
    {
        return id == other.id && generation == other.generation;
    }

    explicit operator bool() const { return id!=0; }
};

} // namespace TLETC::ECS