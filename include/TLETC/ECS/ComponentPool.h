#pragma once

#include "TLETC/ECS/ComponentStorage.h"
#include "TLETC/Core/Containers/PackedArray.h"
#include "TLETC/Core/Assert.h"

namespace TLETC::ECS
{
/**
 * ComponentPool<T>
 * 
 * Stores all components of type T
 * Uses PackedArray for cache-friendly iteration
 */
template<typename T>
class ComponentPool : public IComponentStorage
{
public:
    ComponentPool() = default;
    ~ComponentPool() override = default;

    // Add component
    T& Add(Entity entity, const T& component) 
    {
        return components_.Add(entity.Index(), component);
    }

    T& Add(Entity entity, T&& component) 
    {
        return components_.Add(entity.Index(), std::move(component));
    }

    // Emplace component
    template<typename... Args>
    T& Emplace(Entity entity, Args&&... args)
    {
        return components_.Emplace(entity.Index(), std::forward<Args>(args)...);
    }

    // Remove component
    void Remove(Entity entity) override
    {
        components_.Remove(entity.Index());
    }

    // Check if entity has component
    bool Has(Entity entity) const override
    {
        return components_.Has(entity.Index());
    }

    // Get component (may return nullptr)
    T* Get(Entity entity)
    {
        return components_.Get(entity.Index());
    }
    
    const T* Get(Entity entity) const
    {
        return components_.Get(entity.Index());
    }

    // IComponentStorage interface
    size_t Size() const override { return components_.Size(); }
    void Clear() override { components_.Clear(); }
    
    // Direct access for iteration
    PackedArray<T>& GetArray() { return components_; }
    const PackedArray<T>& GetArray() const { return components_; }

private:
    PackedArray<T> components_;  
};
} // namespace TLETC::ECS
