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
        {
            TLETC_WARN("StackAllocator destroyed with active allocations");
        }
        std::free(buffer_);
        buffer_ = nullptr;
    }
}

} // namespace TLETC::Memory