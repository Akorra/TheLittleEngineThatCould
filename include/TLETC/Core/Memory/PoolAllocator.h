#pragma once

#include "TLETC/Core/Memory/Allocator.h"

namespace TLETC::Memory
{
/**
 * PoolAllocator
 * 
 * - Fixed-size chunks
 * - O(1) allocate and free
 * - Perfect for components (all Transforms are same size)
 * - Minimal fragmentation
 */
class PoolAllocator : public IAllocator
{
public:
    PoolAllocator(size_t chunkSize, size_t chunkCount);
    ~PoolAllocator() override;

    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void Free(void* ptr) override;
    void Reset() override;

    size_t GetAllocatedSize() const override { return allocatedChunks_ * chunkSize_; }
    size_t GetTotalSize() const override { return totalChunks_ * chunkSize_; }

    size_t GetChunkSize() const { return chunkSize_; }
    size_t GetAllocatedChunks() const { return allocatedChunks_; }
    size_t GetFreeChunks() const { return totalChunks_ - allocatedChunks_; }

private:
    void InitializeFreeList();

private:
    void* buffer_;

    size_t chunkSize_;
    size_t allocatedChunks_;
    size_t totalChunks_;

    void* freeListHead_; // points to first free chunk
};

/**
 * STL-compatible allocator adapter
 * Allows using PoolAllocator with STL containers
 */
template<typename T>
class STLPoolAllocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    // Required for C++17
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;
    
    // Constructor
    explicit STLPoolAllocator(PoolAllocator& pool) : pool_(&pool) {}
    
    // Copy constructor (different type)
    template<typename U>
    STLPoolAllocator(const STLPoolAllocator<U>& other) : pool_(other.pool_) {}
    
    // Allocate
    T* allocate(size_type n)
    {
        if (n != 1)
        {
            // Pool allocator only handles single objects
            // Fall back to malloc for multi-object allocations
            return static_cast<T*>(std::malloc(n * sizeof(T)));
        }
        
        return static_cast<T*>(pool_->Allocate(sizeof(T), alignof(T)));
    }
    
    // Deallocate
    void deallocate(T* ptr, size_type n)
    {
        if (n != 1)
        {
            std::free(ptr);
        }
        else
        {
            pool_->Free(ptr);
        }
    }
    
    // Comparison
    template<typename U>
    bool operator==(const STLPoolAllocator<U>& other) const
    {
        return pool_ == other.pool_;
    }
    
    template<typename U>
    bool operator!=(const STLPoolAllocator<U>& other) const
    {
        return !(*this == other);
    }
    
    // For rebind (required by STL)
    template<typename U>
    struct rebind
    {
        using other = STLPoolAllocator<U>;
    };
    
    // Allow access to pool_ from other instances
    template<typename U>
    friend class STLPoolAllocator;

private:
    PoolAllocator* pool_;
};

} // namespace TLETC::Memory