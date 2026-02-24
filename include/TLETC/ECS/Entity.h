#pragma once 

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Assert.h"

namespace TLETC::ECS
{
/**
 * Entity
 * 
 * Lightweight handle with generational index
 * 
 * Layout (64-bit ID):
 * - 32 bits: Index (4B entities)    //< OR 24 bits: Index (16M entities max)
 * - 32 bits: Generation (4B reuses) //< OR 8 bits: Generation (255 reuses max)
 */
struct Entity
{
    uint64 id;

    // Extract index from ID
    uint32 Index() const 
    { 
        return static_cast<uint32_t>(id & 0x00000000FFFFFFFF);  // Lower 32 bits
    }

    // Extract generation from ID
    uint32 Generation() const 
    { 
        return static_cast<uint32_t>(id >> 32);  // Upper 32 bits
    }

    // Create from index and generation
    static Entity Create(const uint32 index, const uint32 generation)
    {
        return Entity{ (static_cast<uint64>(generation) << 32) | index };
    }

    // Null entity
    static Entity Null() { return Entity{ 0xFFFFFFFFFFFFFFFF }; }
    bool IsNull() const { return id == 0xFFFFFFFFFFFFFFFF; }

    bool operator==(Entity other) const { return id == other.id; }
    bool operator!=(Entity other) const { return id != other.id; }

    explicit operator bool() const { return !IsNull(); }
};
    
} // namespace TLETC::ECS

// Hash support for unordered_map/set
namespace std
{
    template<>
    struct hash<TLETC::ECS::Entity>
    {
        size_t operator()(TLETC::ECS::Entity e) const noexcept
        {
            return std::hash<TLETC::uint64>{}(e.id);
        }
    };
}