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

} // namespace TLETC::Memory