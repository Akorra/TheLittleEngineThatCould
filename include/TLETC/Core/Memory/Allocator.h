#pragma once

#include "TLETC/Core/Types.h"
#include <cstddef>

namespace TLETC::Memory
{

/**
 * Allocator Interface
 * All custom allocators inherit from this
 */
class IAllocator
{
public:
    virtual ~IAllocator() = default;

    // Allocate Memory (aligned)
    virtual void* Allocator(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;

    // Free memory
    virtual void Free(void* ptr) = 0;

    // Reset all allocations (if supported)
    virtual void Reset() {}

    // Get total allocated bytes
    virtual size_t GetAllocatedSize() const = 0;
    
    // Get total capacity
    virtual size_t GetTotalSize() const = 0;
};

// Helper: Align pointer to boundary
inline void* AlignPointer(void* ptr, size_t alignment)
{
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned);
}

// Helper: Calculate aligned size
inline size_t AlignSize(size_t size, size_t alignment)
{
    return (size + alignment - 1) & ~(alignment - 1);
}

} // namespace TLETC::Memory
