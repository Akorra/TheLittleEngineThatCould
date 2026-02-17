#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/ECS/ComponentRegistry.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace TLETC::ECS
{

// An archetype is a sorted set of component type IDs
using Archetype = std::vector<ComponentTypeID>;

// Hash for Archetype
struct ArchetypeHash
{
    size_t operator()(const Archetype& arch) const
    {
        size_t seed = arch.size();
        for (auto& id : arch)
        {
            // Combine hashes
            seed ^= std::hash<ComponentTypeID>{}(id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

/**
 * ArchetypeIndex
 *
 * Tracks which entities belong to which archetype.
 * Allows View to skip entire archetypes that don't match query.
 */
class ArchetypeIndex
{
public:
    // Called when a component is added to entity
    void OnComponentAdded(Entity entity, ComponentTypeID typeID)
    {
        auto& archetype = entityArchetypes_[entity];
        
        // Insert in sorted order
        auto it = std::lower_bound(archetype.begin(), archetype.end(), typeID);
        archetype.insert(it, typeID);
        
        // Update archetype -> entity set mapping
        archetypeEntities_[archetype].insert(entity);
    }

    // Called when a component is removed from entity
    void OnComponentRemoved(Entity entity, ComponentTypeID typeID)
    {
        auto& archetype = entityArchetypes_[entity];
        
        // Remove from old archetype group
        archetypeEntities_[archetype].erase(entity);
        
        // Remove component from archetype
        auto it = std::lower_bound(archetype.begin(), archetype.end(), typeID);
        if (it != archetype.end() && *it == typeID)
            archetype.erase(it);
        
        // Add to new archetype group
        archetypeEntities_[archetype].insert(entity);
    }

    // Get all entities that have AT LEAST the queried components
    std::vector<Entity> QueryEntities(const Archetype& query) const
    {
        std::vector<Entity> result;

        for (auto& [archetype, entities] : archetypeEntities_)
        {
            // Check if archetype contains all query components
            bool matches = std::includes(
                archetype.begin(), archetype.end(),
                query.begin(), query.end());

            if (matches)
                result.insert(result.end(), entities.begin(), entities.end());
        }

        return result;
    }

private:
    // Entity → its archetype (sorted component IDs)
    std::unordered_map<Entity, Archetype>                           entityArchetypes_;
    // Archetype → all entities with that exact archetype
    std::unordered_map<Archetype, std::unordered_set<Entity>, ArchetypeHash> archetypeEntities_;
};

} // namespace TLETC::ECS