#include "TLETC/Core/Memory/PoolAllocator.h"
#include "TLETC/Core/Log.h"
#include <cstdlib>

namespace TLETC::Memory
{

PoolAllocator::PoolAllocator(size_t chunkSize, size_t chunkCount) 
    : chunkSize_(chunkSize)
    , totalChunks_(chunkCount)
    , allocatedChunks_(0)
    , freeListHead_(nullptr)
{
    // Ensure chunk size can hold a pointer (for free list)
    if (chunkSize_ < sizeof(void*))
        chunkSize_ = sizeof(void*);

    // Align chunk size to pointer size:
    chunkSize_ = AlignSize(chunkSize_, alignof(void*));

    // Allocate buffer
    size_t totalSize = chunkSize_ * totalChunks_;
    buffer_ = std::malloc(totalSize);
    TLETC_VERIFY(buffer_, "Failed to allocate pool buffer");

    InitializeFreeList();
    
    TLETC_INFO("PoolAllocator created: ",
               totalChunks_, " chunks of ", chunkSize_, " bytes (",
               totalSize / 1024, " KB total)");
}

PoolAllocator::~PoolAllocator()
{
    if (buffer_)
    {
        if (allocatedChunks_ > 0)
            TLETC_WARN("PoolAllocator destroyed with ", allocatedChunks_, " chunks still allocated (memory leak!)");
        
        std::free(buffer_);
        buffer_ = nullptr;
    }
}

void* PoolAllocator::Allocate(size_t size, size_t alignment)
{
    // Pool allocator only handles one size
    TLETC_ASSERT(size <= chunkSize_, "Requested size exceeds chunk size");
    
    // Check if we have free chunks
    if (!freeListHead_)
    {
        TLETC_ERROR("PoolAllocator out of chunks!");
        return nullptr;
    }
    
    // Pop from free list
    void* chunk = freeListHead_;
    freeListHead_ = *reinterpret_cast<void**>(freeListHead_);
    
    allocatedChunks_++;
    return chunk;
}

void PoolAllocator::Free(void* ptr)
{
    if (!ptr)
        return;
    
    // Verify pointer is in our buffer
    #ifdef TLETC_DEBUG
    {
        uintptr_t ptrAddr = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t bufferStart = reinterpret_cast<uintptr_t>(buffer_);
        uintptr_t bufferEnd = bufferStart + (chunkSize_ * totalChunks_);
        
        TLETC_ASSERT(ptrAddr >= bufferStart && ptrAddr < bufferEnd,
                     "Pointer not from this pool!");
        
        // Check alignment
        TLETC_ASSERT((ptrAddr - bufferStart) % chunkSize_ == 0,
                     "Pointer not aligned to chunk boundary!");
    }
    #endif
    
    // Push back to free list
    void** next = reinterpret_cast<void**>(ptr);
    *next = freeListHead_;
    freeListHead_ = ptr;
    
    allocatedChunks_--;
}
void PoolAllocator::Reset()
{
    allocatedChunks_ = 0;
    InitializeFreeList();
}

void PoolAllocator::InitializeFreeList()
{
    // Build linked list through chunks
    char* current = static_cast<char*>(buffer_);
    
    for (size_t i = 0; i < totalChunks_ - 1; ++i)
    {
        // Store pointer to next chunk at start of current chunk
        void** next = reinterpret_cast<void**>(current);
        *next = current + chunkSize_;
        current += chunkSize_;
    }
    
    // Last chunk points to null
    void** last = reinterpret_cast<void**>(current);
    *last = nullptr;
    
    freeListHead_ = buffer_;
}

} // namespace TLETC::Memory