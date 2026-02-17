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

} // namespace TLETC::Memory