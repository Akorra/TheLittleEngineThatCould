#include "TLETC/Core/Memory/AllocationTracker.h"
#include "TLETC/Core/Log.h"
#include <algorithm>

namespace TLETC::Memory
{

void AllocationTracker::RecordAllocation(void* ptr, size_t size,
                                        const char* file, int line, const char* function)
{
    if (!ptr) return;
    
    AllocationInfo info;
    info.ptr = ptr;
    info.size = size;
    info.file = file;
    info.line = line;
    info.function = function;
    
    allocations_[ptr] = info;
    totalAllocated_ += size;
}

void AllocationTracker::RecordFree(void* ptr)
{
    if (!ptr) return;
    
    auto it = allocations_.find(ptr);
    if (it != allocations_.end())
    {
        totalAllocated_ -= it->second.size;
        allocations_.erase(it);
    }
}

void AllocationTracker::PrintTopAllocations(int count)
{
    TLETC_INFO("=== Top ", count, " Allocations ===");
    
    // Sort by size
    std::vector<AllocationInfo> sorted;
    sorted.reserve(allocations_.size());
    
    for (const auto& [ptr, info] : allocations_)
        sorted.push_back(info);
    
    std::sort(sorted.begin(), sorted.end(),
        [](const AllocationInfo& a, const AllocationInfo& b) {
            return a.size > b.size;
        });
    
    int printed = 0;
    for (const auto& info : sorted)
    {
        if (printed++ >= count) break;
        
        TLETC_INFO(printed, ". ", info.size, " bytes at ", info.ptr);
        TLETC_INFO("   ", info.file, ":", info.line, " in ", info.function);
    }
}

void AllocationTracker::PrintLeaks()
{
    if (allocations_.empty())
    {
        TLETC_INFO("No memory leaks detected!");
        return;
    }
    
    TLETC_ERROR("=== MEMORY LEAKS DETECTED ===");
    TLETC_ERROR("Total leaks: ", allocations_.size());
    TLETC_ERROR("Total leaked bytes: ", totalAllocated_);
    
    for (const auto& [ptr, info] : allocations_)
    {
        TLETC_ERROR("Leak: ", info.size, " bytes at ", ptr);
        TLETC_ERROR("  Allocated at ", info.file, ":", info.line);
    }
}

void AllocationTracker::Reset()
{
    allocations_.clear();
    totalAllocated_ = 0;
}

} // namespace TLETC::Memory