#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/ECS/ComponentPool.h"
#include "TLETC/ECS/ComponentRegistry.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Core/Log.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace TLETC::ECS
{
/**
 * EntityRecord
 * Tracks entity state and generation
 */
struct EntityRecord
{
    uint32 generation = 0;
    bool   alive = false;
};

/**
 * Scene
 * 
 * The ECS "world" that owns all entities and components
 * 
 * Responsibilities:
 * - Create/destroy entities
 * - Add/remove/get components
 * - Query entities by components (View)
 */
class Scene
{
public:
    Scene();
    ~Scene();

    // ===== Entity Management =====
    
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    bool IsValid(Entity entity) const;

    // ===== Component Management =====
    
    // Add component
    template<typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args);
    
    // Remove component
    template<typename T>
    void RemoveComponent(Entity entity);
    
    // Get component (may return nullptr)
    template<typename T>
    T* GetComponent(Entity entity);
    
    template<typename T>
    const T* GetComponent(Entity entity) const;
    
    // Check if entity has component
    template<typename T>
    bool HasComponent(Entity entity) const;

    // ===== Queries =====
    
    // View: Iterate over entities with specific components
    template<typename... Components, typename Func>
    void View(Func&& func);

    // ==== Callbakcs ====
    template<typename T>
    void OnComponentAdded(std::function<void(Entity, T&)> callback)
    {
        GetOrCreatePool<T>()->OnAdded(std::move(callback));
    }

    template<typename T>
    void OnComponentRemoved(std::function<void(Entity)> callback)
    {
        GetOrCreatePool<T>()->OnRemoved(std::move(callback));
    }

    // ===== Prefabs =====
    Entity Instantiate(const class Prefab& prefab);
    void   Instantiate(const Prefab& prefab, uint32 count);
    
    // ===== Utility =====
    
    size_t GetEntityCount() const;
    size_t GetAliveEntityCount() const;
    
    void Clear();
    
private:
    // Get or create component pool
    template<typename T>
    ComponentPool<T>* GetOrCreatePool();
    
    // Get component pool (may return nullptr)
    template<typename T>
    ComponentPool<T>* GetPool();
    
    template<typename T>
    const ComponentPool<T>* GetPool() const;
    
    // Helper: Get smallest pool for query optimization
    template<typename... Components>
    IComponentStorage* GetSmallestPool();

private:
    std::vector<EntityRecord> entities_;
    std::vector<uint32>       freeList_;  // Reuse destroyed entity indices
    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentStorage>> componentPools_;
};

// ===== Template Implementations =====

template<typename T, typename... Args>
T& Scene::AddComponent(Entity entity, Args&&... args)
{
    TLETC_ASSERT(IsValid(entity), "Entity is not valid!");
    
    ComponentPool<T>* pool = GetOrCreatePool<T>();
    TLETC_ASSERT(!pool->Has(entity), "Entity already has this component!");
    
    return pool->Emplace(entity, std::forward<Args>(args)...);
}

template<typename T>
void Scene::RemoveComponent(Entity entity)
{
    if (!IsValid(entity))
        return;
    
    ComponentPool<T>* pool = GetPool<T>();
    if (pool)
        pool->Remove(entity);
}

template<typename T>
T* Scene::GetComponent(Entity entity)
{
    if (!IsValid(entity))
        return nullptr;
    
    ComponentPool<T>* pool = GetPool<T>();
    return pool ? pool->Get(entity) : nullptr;
}

template<typename T>
const T* Scene::GetComponent(Entity entity) const
{
    if (!IsValid(entity))
        return nullptr;
    
    const ComponentPool<T>* pool = GetPool<T>();
    return pool ? pool->Get(entity) : nullptr;
}

template<typename T>
bool Scene::HasComponent(Entity entity) const
{
    if (!IsValid(entity))
        return false;
    
    const ComponentPool<T>* pool = GetPool<T>();
    return pool ? pool->Has(entity) : false;
}

template<typename... Components, typename Func>
void Scene::View(Func&& func)
{
    // Get smallest pool for optimization
    IComponentStorage* smallest = GetSmallestPool<Components...>();
    if (!smallest || smallest->Size() == 0)
        return;
    
    // We need to get ALL pools to iterate properly
    // For now, let's use a simple approach: get first pool and check others
    
    // Get first component pool TODO: use smallest pool
    ComponentPool<typename std::tuple_element<0, std::tuple<Components...>>::type>* firstPool = 
        GetPool<typename std::tuple_element<0, std::tuple<Components...>>::type>();
    
    if (!firstPool)
        return;
    
    // Iterate through first pool
    auto& arr = firstPool->GetArray();
    for (size_t i = 0; i < arr.Size(); ++i)
    {
        uint32 entityIndex = arr.GetEntityID(i);
        Entity entity = Entity::Create(entityIndex, entities_[entityIndex].generation);
        
        // Check if entity has ALL required components
        bool hasAll = (HasComponent<Components>(entity) && ...);
        
        if (hasAll)
        {
            // Call function with entity and all components
            func(entity, *GetComponent<Components>(entity)...);
        }
    }
}

template<typename T>
ComponentPool<T>* Scene::GetOrCreatePool()
{
    ComponentTypeID typeID = GetComponentTypeID<T>();
    
    auto it = componentPools_.find(typeID);
    if (it == componentPools_.end())
    {
        // Create new pool
        auto pool = std::make_unique<ComponentPool<T>>();
        ComponentPool<T>* ptr = pool.get();
        componentPools_[typeID] = std::move(pool);
        
        TLETC_TRACE("Created component pool for type: ", GetComponentTypeName<T>());
        return ptr;
    }
    
    return static_cast<ComponentPool<T>*>(it->second.get());
}

template<typename T>
ComponentPool<T>* Scene::GetPool()
{
    ComponentTypeID typeID = GetComponentTypeID<T>();
    
    auto it = componentPools_.find(typeID);
    if (it == componentPools_.end())
        return nullptr;
    
    return static_cast<ComponentPool<T>*>(it->second.get());
}

template<typename T>
const ComponentPool<T>* Scene::GetPool() const
{
    ComponentTypeID typeID = GetComponentTypeID<T>();
    
    auto it = componentPools_.find(typeID);
    if (it == componentPools_.end())
        return nullptr;
    
    return static_cast<const ComponentPool<T>*>(it->second.get());
}

template<typename... Components>
IComponentStorage* Scene::GetSmallestPool()
{
    IComponentStorage* smallest = nullptr;
    size_t smallestSize = SIZE_MAX;
    
    // Fold expression to check each component pool
    auto checkPool = [&]<typename T>() {
        ComponentPool<T>* pool = GetPool<T>();
        if (!pool)
            return;
        
        if (pool->Size() < smallestSize)
        {
            smallestSize = pool->Size();
            smallest = pool;
        }
    };
    
    (checkPool.template operator()<Components>(), ...);
    
    return smallest;
}

} // namespace TLETC::ECS
