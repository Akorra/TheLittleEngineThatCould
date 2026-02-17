#include "TLETC/Core/Memory/DoubleEndedStackAllocator.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Core/Log.h"

namespace TLETC::Memory
{

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

    auto alignedOffset = AlignSize(bottomOffset_, alignment);
    if(alignedOffset + totalSize > topOffset_)
    {
        TLETC_ERROR("StackAllocator out of memory!");
        return nullptr;
    }

    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(buffer_) + alignedOffset);
    header->prevOffset = bottomPrevOffset_;
    header->size = size;

    bottomPrevOffset_ = alignedOffset;
    bottomOffset_     = alignedOffset + totalSize;  

    return header + 1;
}

void* DoubleEndedStackAllocator::AllocateTop(size_t size, size_t alignment)
{
    // Calculate header + data size
    size_t dataOffset = topOffset_ - size;
    size_t alignedOffset = dataOffset & ~(alignment - 1); // round offset down to the closest alignment that fits our memory
    size_t headerOffset = alignedOffset - sizeof(AllocationHeader);

    // Check space
    if (headerOffset < bottomOffset_)
    {
        TLETC_ERROR("StackAllocator out of memory!");
        return nullptr;
    }
    
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(buffer_) + headerOffset);
    header->prevOffset = topPrevOffset_;
    header->size = size;

    topPrevOffset_ = headerOffset;
    topOffset_     = headerOffset;  

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