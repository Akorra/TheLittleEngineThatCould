#include "TLETC/Core/Memory/LinearAllocator.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Core/Log.h"

#include <cstdlib>
#include <cstring>

namespace TLETC::Memory
{
LinearAllocator::LinearAllocator(size_t size) : totalSize_(size), currentOffset_(0)
{
    buffer_ = std::malloc(size);
    TLETC_VERIFY(buffer_, "Failed to allocate linear buffer");
    TLETC_INFO("LinearAllocator created: ", size / 1024, " KB");
}

LinearAllocator::~LinearAllocator()
{
    if (buffer_)
    {
        std::free(buffer_);
        buffer_ = nullptr;
    }
}

void* LinearAllocator::Allocate(size_t size, size_t alignment)
{
    // Calculate aligned offset
    size_t alignedOffset = AlignSize(currentOffset_, alignment);
    
    // Check if we have space
    if (alignedOffset + size > totalSize_)
    {
        TLETC_ERROR("LinearAllocator out of memory! ",
                    "Requested: ", size, " bytes, ",
                    "Available: ", totalSize_ - alignedOffset, " bytes");
        return nullptr;
    }
    
    // Return pointer and bump offset
    void* ptr = static_cast<char*>(buffer_) + alignedOffset;
    currentOffset_ = alignedOffset + size;
    
    return ptr;
}

void LinearAllocator::Free(void* ptr)
{
    // Linear allocator doesn't support individual free
    // Use Reset() to free everything
}

void LinearAllocator::Reset()
{
    currentOffset_ = 0;
    // Optionally: memset buffer to zero for debugging
    #ifdef TLETC_DEBUG
        std::memset(buffer_, 0, totalSize_);
    #endif
}

} // namespace TLETC::Memory