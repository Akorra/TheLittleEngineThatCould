#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Memory/PoolAllocator.h"
#include "TLETC/Core/Assert.h"

#include <vector>
#include <unordered_map>

namespace TLETC
{
/**
 * PackedArray<T>
 * 
 * Cache-friendly component storage:
 * - Components stored in contiguous array
 * - Entity IDs stored in parallel array
 * - Sparse lookup map for O(1) access
 * 
 * Iteration is FAST (sequential memory)
 * Random access is still O(1)
 */
template<typename T>
class PackedArray
{
public:
    PackedArray() = default;
    ~PackedArray() { Clear(); }
    
    // Add component
    T& Add(uint32 entityID, const T& component)
    {
        TLETC_ASSERT(!Has(entityID), "Entity already has component");
        
        size_t index = components_.size();
        
        components_.push_back(component);
        entityIDs_.push_back(entityID);
        sparse_[entityID] = index;
        
        return components_.back();
    }

    // Add component (move)
    T& Add(uint32 entityID, T&& component)
    {
        TLETC_ASSERT(!Has(entityID), "Entity already has component");
        
        size_t index = components_.size();
        
        components_.push_back(std::move(component));
        entityIDs_.push_back(entityID);
        sparse_[entityID] = index;
        
        return components_.back();
    }

    // Emplace component
    template<typename... Args>
    T& Emplace(uint32 entityID, Args&&... args)
    {
        TLETC_ASSERT(!Has(entityID), "Entity already has component");
        
        size_t index = components_.size();
        
        components_.emplace_back(std::forward<Args>(args)...);
        entityIDs_.push_back(entityID);
        sparse_[entityID] = index;
        
        return components_.back();
    }
    
    // Remove component (swap with last)
    void Remove(uint32 entityID)
    {
        auto it = sparse_.find(entityID);
        if (it == sparse_.end())
            return;
        
        size_t index = it->second;
        size_t lastIndex = components_.size() - 1;
        
        if (index != lastIndex)
        {
            // Swap with last
            components_[index] = std::move(components_[lastIndex]);
            entityIDs_[index] = entityIDs_[lastIndex];
            
            // Update sparse lookup for swapped entity
            sparse_[entityIDs_[index]] = index;
        }
        
        components_.pop_back();
        entityIDs_.pop_back();
        sparse_.erase(it);
    }
    
    // Check if entity has component
    bool Has(uint32 entityID) const
    {
        return sparse_.find(entityID) != sparse_.end();
    }
    
    // Get component (may return nullptr)
    T* Get(uint32 entityID)
    {
        auto it = sparse_.find(entityID);
        if (it == sparse_.end())
            return nullptr;
        return &components_[it->second];
    }
    
    const T* Get(uint32 entityID) const
    {
        auto it = sparse_.find(entityID);
        if (it == sparse_.end())
            return nullptr;
        return &components_[it->second];
    }
    
    // Iteration
    size_t Size() const { return components_.size(); }
    bool Empty() const { return components_.empty(); }
    
    T& operator[](size_t index) { return components_[index]; }
    const T& operator[](size_t index) const { return components_[index]; }
    
    uint32 GetEntityID(size_t index) const { return entityIDs_[index]; }
    
    // Direct array access for iteration
    T* Data() { return components_.data(); }
    const T* Data() const { return components_.data(); }
    
    // Iterators
    auto begin() { return components_.begin(); }
    auto end() { return components_.end(); }
    auto begin() const { return components_.begin(); }
    auto end() const { return components_.end(); }
    
    // Clear all
    void Clear()
    {
        components_.clear();
        entityIDs_.clear();
        sparse_.clear();
    }

private:
    std::vector<T> components_;                  //< Dense array of components
    std::vector<uint32> entityIDs_;              //< Parallel array of entity IDs
    std::unordered_map<uint32, size_t> sparse_;  //< EntityID -> index lookup
    // TODO: Thinkabout it : std::vector<size_t> sparce_; //< initialize with INVALID (0xFFFFFFFFFFFFFFFF)
};

} // namespace TLETC
