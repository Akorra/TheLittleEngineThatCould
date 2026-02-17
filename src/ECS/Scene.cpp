#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/Prefab.h"

namespace TLETC::ECS
{

Scene::Scene()
{
    TLETC_INFO("Scene created");
}

Scene::~Scene()
{
    Clear();
    TLETC_INFO("Scene destroyed");
}

Entity Scene::CreateEntity()
{
    uint32 index;
    uint32 generation;
    
    if (!freeList_.empty())
    {
        // Reuse destroyed entity index
        index = freeList_.back();
        freeList_.pop_back();
        
        auto& record = entities_[index];
        record.generation++;  // Increment generation
        record.alive = true;
        generation = record.generation;
    }
    else
    {
        // Create new entity
        index = static_cast<uint32>(entities_.size());
        
        if (index >= 0x00FFFFFF)
        {
            TLETC_ERROR("Entity index overflow! Max entities: ", 0x00FFFFFF);
            return Entity::Null();
        }
        
        entities_.push_back(EntityRecord{ 0, true });
        generation = 0;
    }
    
    return Entity::Create(index, generation);
}

void Scene::DestroyEntity(Entity entity)
{
    if (!IsValid(entity))
        return;
    
    // Remove from all component pools
    for (auto& [typeID, pool] : componentPools_)
    {
        pool->Remove(entity);
    }
    
    // Mark as dead
    auto& record = entities_[entity.Index()];
    record.alive = false;
    
    // Add to free list
    freeList_.push_back(entity.Index());
}

bool Scene::IsValid(Entity entity) const
{
    if (entity.IsNull())
        return false;
    
    uint32 index = entity.Index();
    if (index >= entities_.size())
        return false;
    
    const auto& record = entities_[index];
    return record.alive && record.generation == entity.Generation();
}

Entity Scene::Instantiate(const Prefab& prefab)
{
    Entity e = CreateEntity();
    prefab.Apply(*this, e);
    return e;
}

void Scene::Instantiate(const Prefab& prefab, uint32 count)
{
    for (uint32 i = 0; i < count; ++i)
        Instantiate(prefab);
}

size_t Scene::GetEntityCount() const
{
    return entities_.size();
}

size_t Scene::GetAliveEntityCount() const
{
    return entities_.size() - freeList_.size();
}

void Scene::Clear()
{
    componentPools_.clear();
    entities_.clear();
    freeList_.clear();
}

} // namespace TLETC::ECS