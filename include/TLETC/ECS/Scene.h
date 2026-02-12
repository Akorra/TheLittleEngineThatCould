#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/ECS/System.h"

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>

namespace TLETC::ECS 
{

// =======================================================
// Internal Entity Record
// =======================================================

struct EntityRecord
{
    uint32_t generation = 0;
    bool     alive      = false;
};

// =======================================================
// Component Pool Base
// =======================================================
class IComponentPool
{
public:
    virtual ~IComponentPool() = default;
    virtual void Remove(EntityID id) = 0;
};

// =======================================================
// Typed Component Pool
// =======================================================

template<typename T>
class ComponentPool : public IComponentPool
{
public:
    std::vector<T> components;
    std::vector<EntityID> entities;
    std::unordered_map<EntityID, size_t> lookup;

    void Remove(EntityID id) override
    {
        auto it = lookup.find(id);
        if (it == lookup.end())
            return;

        size_t index = it->second;                  //< Index of entity to remove
        size_t lastIndex = components.size() - 1;

        // Swap with last
        components[index] = std::move(components[lastIndex]);
        entities[index] = entities[lastIndex];

        // update lookup for moved entity
        lookup[entities[index]] = index;

        // remove last element from dense arrays
        components.pop_back();
        entities.pop_back();

        // Remove lookup entry for removed entity
        lookup.erase(it);
    }

    bool Has(EntityID id) const
    {
        return lookup.find(id) != lookup.end();
    }

    T* Get(EntityID id)
    {
        auto it = lookup.find(id);
        if (it == lookup.end())
            return nullptr;

        return &components[it->second];
    }
};

// =======================================================
// Scene
// =======================================================

/**
 * Scene 
 * Owns entities and components
 * Registers systems
 * Updates them in order
 */
class Scene
{
public:
    // Entity Management ---------------------------------------------------------
    Entity CreateEntity()
    {
        EntityID id;

        if (!freeList_.empty())
        {
            id = freeList_.back();
            freeList_.pop_back();
        }
        else
        {
            id = static_cast<EntityID>(entities_.size());
            entities_.emplace_back();
        }

        auto& record = entities_[id];
        record.alive = true;

        return Entity{ id, record.generation };
    }

    void DestroyEntity(Entity e)
    {
        if (!IsValid(e))
            return;

        // Remove from all pools
        for (auto& [_, pool] : pools_)
            pool->Remove(e.id);

        auto& record = entities_[e.id];
        record.alive = false;
        record.generation++;

        freeList_.push_back(e.id);
    }

    bool IsValid(Entity e) const
    {
        if (e.id >= entities_.size())
            return false;

        const auto& record = entities_[e.id];
        return record.alive && record.generation == e.generation;
    }

    // Component Management -----------------------------------------------------
    template<typename T, typename... Args>
    T& AddComponent(Entity e, Args&&... args)
    {
        assert(IsValid(e));

        auto& pool = GetOrCreatePool<T>();

        // prevent dupplicate component
        assert(!pool.Has(e.id) && "Entity already has component");

        pool.components.emplace_back(std::forward<Args>(args)...);
        pool.entities.push_back(e.id);

        size_t index = pool.components.size() - 1;
        pool.lookup[e.id] = index;

        return pool.components.back();
    }

    template<typename T>
    T* GetComponent(Entity e)
    {
        if (!IsValid(e))
            return nullptr;

        auto* pool = GetPool<T>();
        if (!pool)
            return nullptr;

        return pool->Get(e.id);
    }

    template<typename T>
    void RemoveComponent(Entity e)
    {
        if (!IsValid(e))
            return;

        auto* pool = GetPool<T>();
        if (!pool)
            return;

        pool->Remove(e.id);
    }

    template<typename T>
    bool HasComponent(Entity e) const
    {
        if (!IsValid(e))
            return false;

        auto* pool = GetPool<T>();
        if (!pool)
            return false;

        return pool->Has(e.id);
    }

    // View System ----------------------------------------------------------------
    template<typename... Components, typename Func>
    void View(Func&& func)
    {
        auto* smallestPool = GetSmallestPool<Components...>();
        if (!smallestPool)
            return;

        for (size_t i = 0; i < smallestPool->entities.size(); ++i)
        {
            EntityID id = smallestPool->entities[i];
            Entity    e{ id, entities_[id].generation }
            
            if(!IsValid(e))
                continue;

            if ((HasComponent<Components>(e) && ...))
            {
                func( e, *GetComponent<Components>(e)... );
            }
        }
    }

private:
    // Pool Access Helpers

    template<typename T>
    ComponentPool<T>& GetOrCreatePool()
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = pools_.find(type);
        if (it == pools_.end())
        {
            auto pool = MakeUnique<ComponentPool<T>>();
            pools_[type] = std::move(pool);
        }

        return *static_cast<ComponentPool<T>*>(pools_[type].get());
    }

    template<typename T>
    ComponentPool<T>* GetPool()
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = pools_.find(type);
        if (it == pools_.end())
            return nullptr;

        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    const ComponentPool<T>* GetPool() const
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = pools_.find(type);
        if (it == pools_.end())
            return nullptr;

        return static_cast<const ComponentPool<T>*>(it->second.get());
    }

    template<typename... Components>
    IComponentPool* GetSmallestPool()
    {
        IComponentPool* result = nullptr;
        size_t smallestSize = SIZE_MAX;

        (SelectSmallestPool<Components>(result, smallestSize), ...);

        return result;
    }

    template<typename T>
    void SelectSmallestPool(IComponentPool*& result, size_t& smallestSize)
    {
        auto* pool = GetPool<T>();
        if (!pool)
            return;

        if (pool->components.size() < smallestSize)
        {
            smallestSize = pool->components.size();
            result = pool;
        }
    }

private:

    std::vector<EntityRecord> entities_;
    std::vector<EntityID> freeList_;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
};

} // namespace TLETC::ECS