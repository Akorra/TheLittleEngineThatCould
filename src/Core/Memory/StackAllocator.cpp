#include "TLETC/Core/Memory/StackAllocator.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Core/Log.h"
#include <cstdlib>

namespace TLETC::Memory
{

StackAllocator::StackAllocator(size_t size)
    : totalSize_(size)
    , currentOffset_(0)
    , prevOffset_(0)
{
    buffer_ = std::malloc(size);
    TLETC_VERIFY(buffer_, "Failed to allocate stack buffer");
    TLETC_INFO("StackAllocator created: ", size / 1024, " KB");
}

StackAllocator::~StackAllocator()
{
    if (buffer_)
    {
        if (currentOffset_ > 0)
            TLETC_WARN("StackAllocator destroyed with active allocations");

        std::free(buffer_);
        buffer_ = nullptr;
    }
}

void* StackAllocator::Allocate(size_t size, size_t alignment)
{
    // Calculate header + data size
    size_t headerSize = sizeof(AllocationHeader);
    size_t totalSize = headerSize + size;
    
    // Align offset
    size_t alignedOffset = AlignSize(currentOffset_, alignment);
    
    // Check space
    if (alignedOffset + totalSize > totalSize_)
    {
        TLETC_ERROR("StackAllocator out of memory!");
        return nullptr;
    }
    
    // Write header
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(buffer_) + alignedOffset);
    header->size = size;
    header->prevOffset = prevOffset_;
    
    // Update offsets
    prevOffset_ = alignedOffset;
    currentOffset_ = alignedOffset + totalSize;
    
    // Return data pointer (after header)
    return header + 1;
}

void StackAllocator::Free(void* ptr)
{
    if (!ptr)
        return;
    
    // Get header
    AllocationHeader* header = static_cast<AllocationHeader*>(ptr) - 1;
    
    // Verify this is the top allocation
    size_t headerOffset = reinterpret_cast<char*>(header) - static_cast<char*>(buffer_);
    TLETC_ASSERT(headerOffset == prevOffset_, "Stack allocator: must free in LIFO order!");
    
    // Restore previous offset
    currentOffset_ = prevOffset_;
    prevOffset_ = header->prevOffset;
}

void StackAllocator::Reset()
{
    currentOffset_ = 0;
    prevOffset_ = 0;
}

void StackAllocator::FreeToMarker(Marker marker)
{
    TLETC_ASSERT(marker.offset <= currentOffset_, "Invalid marker!");
    currentOffset_ = marker.offset;
    
    // Note: prevOffset_ becomes invalid here, but that's okay
    // as long as we don't Free() individual pointers
}

//----------------------------------------------------------------------------------------------------
// Double Ended Stack Allocator
//----------------------------------------------------------------------------------------------------

DoubleEndedStackAllocator::DoubleEndedStackAllocator(size_t size)
    : totalSize_(size)
    , bottomOffset_(0)
    , bottomPrevOffset_(0)
    , topOffset_(size)
    , topPrevOffset_(size)
{
    buffer_ = std::malloc(size);
    TLETC_VERIFY(buffer_, "Failed to double-ended allocate stack buffer");
    TLETC_INFO("DoubleEndedStackAllocator created: ", size / 1024, " KB");
}

DoubleEndedStackAllocator::~DoubleEndedStackAllocator()
{
    if (buffer_)
    {
        std::free(buffer_);
        buffer_ = nullptr;
    }
}

void* DoubleEndedStackAllocator::Allocate(size_t size, size_t alignment)
{
    return AllocateBottom(size, alignment);
}

void DoubleEndedStackAllocator::Free(void* ptr)
{
    FreeBottom(ptr);
}

void DoubleEndedStackAllocator::Reset()
{
    ResetBottom();
    ResetTop();
}

void* DoubleEndedStackAllocator::AllocateBottom(size_t size, size_t alignment)
{
    // Calculate header + data size
    size_t headerSize = sizeof(AllocationHeader);
    size_t totalSize = headerSize + size;
    
    // Align offset
    size_t alignedOffset = AlignSize(bottomOffset_, alignment);
    
    // Check space
    if (alignedOffset + totalSize > topOffset_)
    {
        TLETC_ERROR("StackAllocator out of memory!");
        return nullptr;
    }
    
    // Write header
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(buffer_) + alignedOffset);
    header->size = size;
    header->prevOffset = bottomPrevOffset_;
    
    // Update offsets
    bottomPrevOffset_ = alignedOffset;
    bottomOffset_ = alignedOffset + totalSize;
    
    // Return data pointer (after header)
    return header + 1;
}

void* DoubleEndedStackAllocator::AllocateTop(size_t size, size_t alignment)
{
    // Calculate header + data size
    size_t dataOffset = topOffset_ - size;
    size_t alignedDataOffset = dataOffset & ~(alignment - 1);
    size_t headerOffset = alignedDataOffset - sizeof(AllocationHeader);
    
    // Check space
    if (headerOffset < bottomOffset_)
    {
        TLETC_ERROR("StackAllocator out of memory!");
        return nullptr;
    }
    
    // Write header
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(buffer_) + headerOffset);
    header->size = size;
    header->prevOffset = topPrevOffset_;
    
    // Update offsets
    topPrevOffset_ = headerOffset;
    topOffset_ = headerOffset;
    
    // Return data pointer (after header)
    return header + 1;
}

void DoubleEndedStackAllocator::FreeBottom(void* ptr)
{
    if (!ptr) return;
    
    AllocationHeader* header = static_cast<AllocationHeader*>(ptr) - 1;
    
    size_t headerOffset = reinterpret_cast<char*>(header) - static_cast<char*>(buffer_);
    TLETC_ASSERT(headerOffset == bottomPrevOffset_, "Bottom stack: must free in LIFO order!");
    
    bottomOffset_ = bottomPrevOffset_;
    bottomPrevOffset_ = header->prevOffset;
}

void DoubleEndedStackAllocator::FreeTop(void* ptr)
{
    if (!ptr) return;
    
    AllocationHeader* header = static_cast<AllocationHeader*>(ptr) - 1;
    
    size_t headerOffset = reinterpret_cast<char*>(header) - static_cast<char*>(buffer_);
    TLETC_ASSERT(headerOffset == topPrevOffset_, "Top stack: must free in LIFO order!");
    
    topOffset_ = topPrevOffset_ + sizeof(AllocationHeader) + header->size;
    topPrevOffset_ = header->prevOffset;
}  

void DoubleEndedStackAllocator::ResetBottom()
{
    bottomOffset_ = 0;
    bottomPrevOffset_ = 0;
}

void DoubleEndedStackAllocator::ResetTop()
{
    topOffset_ = totalSize_;
    topPrevOffset_ = totalSize_;
}


} // namespace TLETC::Memory