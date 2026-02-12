#include "TLETC/Core/Memory/LinearAllocator.h"
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

} // namespace TLETC::Memory