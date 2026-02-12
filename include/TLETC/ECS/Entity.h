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
    EntityID id = INVALID_ID;
    uint32   generation = 0;

    bool operator==(const Entity& other) const 
    {
        return id == other.id && generation == other.generation;
    }

    static constexpr EntityID INVALID_ID = 0xFFFFFFFF;
    explicit operator bool() const { return id!=INVALID_ID; }
};

} // namespace TLETC::ECS