#pragma once

#include "TLETC/Core/Memory/PoolAllocator.h"

namespace TLETC::Memory
{
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