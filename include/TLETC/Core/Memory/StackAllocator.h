#pragma once

#include "TLETC/Core/Memory/Allocator.h"

namespace TLETC::Memory
{
/**
 * StackAllocator
 * 
 * - LIFO allocation/deallocation
 * - Must free in reverse order
 * - Perfect for scoped temps
 */
class StackAllocator : public IAllocator
{
public:
    explicit StackAllocator(size_t size);
    ~StackAllocator() override;
    
    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void Free(void* ptr) override;
    void Reset() override;
    
    size_t GetAllocatedSize() const override { return currentOffset_; }
    size_t GetTotalSize() const override { return totalSize_; }
    
    // Stack-specific: Save/restore marker
    struct Marker
    {
        size_t offset;
    };
    
    Marker GetMarker() const { return Marker{ currentOffset_ }; }
    void FreeToMarker(Marker marker);

private:
    struct AllocationHeader
    {
        size_t size;
        size_t prevOffset;  // For validation
    };

private:
    void* buffer_;
    size_t totalSize_;
    size_t currentOffset_;
    size_t prevOffset_;   
};

class DoubleEndedStackAllocator : public IAllocator
{
public:
    explicit DoubleEndedStackAllocator(size_t size);
    ~DoubleEndedStackAllocator() override;
    
    // IAllocator interface (allocates from bottom)
    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void Free(void* ptr) override;
    void Reset() override;

    size_t GetAllocatedSize() const override { return bottomOffset_ + (totalSize_ - topOffset_);  }
    size_t GetTotalSize() const override { return totalSize_; }

    // Double-ended specific
    void* AllocateBottom(size_t size, size_t alignment = alignof(std::max_align_t));
    void* AllocateTop(size_t size, size_t alignment = alignof(std::max_align_t));

    void FreeBottom(void* ptr);
    void FreeTop(void* ptr);
    
    void ResetBottom();
    void ResetTop();

    // Get remaining space between stacks
    size_t GetFreeSpace() const { return topOffset_ > bottomOffset_ ? topOffset_ - bottomOffset_ : 0; }

private:
    struct AllocationHeader
    {
        size_t size;
        size_t prevOffset;
    };

private:
    void* buffer_;
    size_t totalSize_;

    size_t bottomOffset_;
    size_t bottomPrevOffset_;

    size_t topOffset_;
    size_t topPrevOffset_;
};

} // namespace TLETC::Memory