#pragma once

#include "TLETC/ECS/ComponentStorage.h"
#include "TLETC/Core/Containers/PackedArray.h"
#include "TLETC/Core/Assert.h"

#include <functional>
#include <vector>

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

    using OnAddCallback    = std::function<void(Entity, T&)>;
    using OnRemoveCallback = std::function<void(Entity)>;

    // --- Callbacks ---
    void OnAdded(OnAddCallback cb)      { onAddCallbacks_.push_back(std::move(cb)); }
    void OnRemoved(OnRemoveCallback cb) { onRemoveCallbacks_.push_back(std::move(cb)); }

    // Add component
    T& Add(Entity entity, const T& component) 
    {
        T& component = components_.Add(entity.Index(), component);

        for (auto& cb : onAddCallbacks_)
            cb(entity, component);
        
        return component;
    }

    T& Add(Entity entity, T&& component) 
    {
        T& component = components_.Add(entity.Index(), std::move(component));

        for (auto& cb : onAddCallbacks_)
            cb(entity, component);
        
        return component;
    }

    // Emplace component
    template<typename... Args>
    T& Emplace(Entity entity, Args&&... args)
    {
        T& component = components_.Emplace(entity.Index(), std::forward<Args>(args)...);

        for (auto& cb : onAddCallbacks_)
            cb(entity, component);
        
        return component;
    }

    // Remove component
    void Remove(Entity entity) override
    {
        if(!components_.Has(entity.Index()))
            return;
        
        for (auto& cb : onRemoveCallbacks_)
            cb(entity);

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
    std::vector<OnAddCallback>    onAddCallbacks_;
    std::vector<OnRemoveCallback> onRemoveCallbacks_;
};
} // namespace TLETC::ECS
