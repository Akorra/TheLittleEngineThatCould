#pragma once

#include "TLETC/ECS/Entity.h"

namespace TLETC::ECS
{
/**
 * IComponentStorage
 * 
 * Type-erased interface for component storage
 * Each component type (Transform, Health, etc.) gets its own storage
 */
class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;
    
    // Remove component for entity
    virtual void Remove(Entity entity) = 0;
    
    // Check if entity has component
    virtual bool Has(Entity entity) const = 0;
    
    // Get number of components
    virtual size_t Size() const = 0;
    
    // Clear all components
    virtual void Clear() = 0;
};

} // namespace TLETC::ECS
