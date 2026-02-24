#include "TLETC/ECS/Scene.h"

#include "TLETC/ECS/Prefab.h"
#include "TLETC/ECS/Components/Transform.h"

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

    // Remove from parent's children list
    Transform* t = GetComponent<Transform>(entity);
    if ( t && !(t->parent_.IsNull()) )
    {
        Transform* parent = GetComponent<Transform>(t->parent_);
        if (parent)
        {
            auto& siblings = parent->children_;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), entity), siblings.end());
        }
    }

    // Orphan children (make children roots)
    if (t)
    {
        for (Entity child : t->children_)
        {
            Transform* childT = GetComponent<Transform>(child);
            if (childT)
            {
                childT->parent_ = Entity::Null();
                childT->dirty_ = true;
            }
        }
    }
    
    // Remove from all component pools
    for (auto& [typeID, pool] : componentPools_)
        pool->Remove(entity);
    
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

void Scene::SetParent(Entity child, Entity parent)
{
    TLETC_ASSERT(IsValid(child), "Child entity is not valid!");
    TLETC_ASSERT(IsValid(parent), "Parent entity is not valid!");
    TLETC_ASSERT(child != parent, "Entity cannot be its own parent!");

    Transform* childTransform = GetComponent<Transform>(child);
    Transform* parentTransform = GetComponent<Transform>(parent);

    if (!childTransform || !parentTransform)
    {
        TLETC_ERROR("Both entities must have Transform component!");
        return;
    }

    // Check for cycles (child is ancestor of parent)
    Entity ancestor = parent;
    while (ancestor)
    {
        if (ancestor == child)
        {
            TLETC_ERROR("Cyclic parenting detected! Cannot set parent.");
            return;
        }

        Transform* t = GetComponent<Transform>(ancestor);
        ancestor = t ? t->parent_ : Entity::Null();
    }

    // Remove from old parent
    if ( !(childTransform->parent_.IsNull()) )
    {
        Transform* oldParent = GetComponent<Transform>(childTransform->parent_);
        if (oldParent)
        {
            auto& siblings = oldParent->children_;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
        }
    }

    // Set new parent
    childTransform->parent_ = parent;
    parentTransform->children_.push_back(child);

    // Mark dirty
    childTransform->dirty_ = true;
}

void Scene::ClearParent(Entity child)
{
    TLETC_ASSERT(IsValid(child), "Child entity is not valid!");

    Transform* childTransform = GetComponent<Transform>(child);
    if (!childTransform)
        return;

    // Remove from old parent's children list
    if ( !(childTransform->parent_.IsNull()) )
    {
        Transform* parent = GetComponent<Transform>(childTransform->parent_);
        if (parent)
        {
            auto& siblings = parent->children_;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
        }
    }

    childTransform->parent_ = Entity::Null();
    childTransform->dirty_ = true;
}

void Scene::DestroyEntityRecursive(Entity entity)
{
    if (!IsValid(entity))
        return;

    Transform* t = GetComponent<Transform>(entity);
    if (t)
    {
        // Destroy children first
        std::vector<Entity> childrenCopy = t->children_;
        for (Entity child : childrenCopy)
            DestroyEntityRecursive(child);
    }

    // Then destroy this entity
    DestroyEntity(entity);
}

std::vector<Entity> Scene::GetRootEntities() const
{
    std::vector<Entity> roots;

    // Iterate all transforms, collect roots
    const ComponentPool<Transform>* pool = GetPool<Transform>();
    if (!pool)
        return roots;

    auto& arr = pool->GetArray();
    for (size_t i = 0; i < arr.Size(); ++i)
    {
        uint32 entityIndex = arr.GetEntityID(i);
        const Transform& t = arr[i];

        if (t.IsRoot())
        {
            Entity e = Entity::Create(entityIndex, entities_[entityIndex].generation);
            roots.push_back(e);
        }
    }

    return roots;
}

void Scene::Clear()
{
    componentPools_.clear();
    entities_.clear();
    freeList_.clear();
}

} // namespace TLETC::ECS