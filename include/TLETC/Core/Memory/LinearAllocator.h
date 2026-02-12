#pragma once

#include "TLETC/Core/Memory/Allocator.h"
#include "TLETC/Core/Assert.h"

namespace TLETC::Memory
{

/**
 * LinearAllocator
 * 
 * - Super fast allocation (just bump pointer)
 * - No individual free (only Reset() to free all)
 * - Perfect for frame temps, scene loading
 */
class LinearAllocator : public IAllocator
{
public:
    explicit LinearAllocator(size_t size);
    ~LinearAllocator() override;
    
    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void  Free(void* ptr) override; // No-op for linear allocator
    void  Reset() override;
    
    size_t GetAllocatedSize() const override { return currentOffset_; }
    size_t GetTotalSize() const override { return totalSize_; }

private:
    void* buffer_;
    size_t totalSize_;
    size_t currentOffset_;
};

} // namespace TLETC::Memory