// TLETC/Core/Memory/AllocationTracker.h
#pragma once

#include "TLETC/Core/Types.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace TLETC::Memory
{

struct AllocationInfo
{
    void* ptr;
    size_t size;
    const char* file;
    int line;
    const char* function;
};

class AllocationTracker
{
public:
    static AllocationTracker& Get()
    {
        static AllocationTracker instance;
        return instance;
    }
    
    void RecordAllocation(void* ptr, size_t size, const char* file, int line, const char* function);
    
    void RecordFree(void* ptr);
    
    void PrintTopAllocations(int count = 10);
    void PrintLeaks();
    void Reset();
    
    size_t GetTotalAllocated() const { return totalAllocated_; }
    size_t GetAllocationCount() const { return allocations_.size(); }

private:
    AllocationTracker() = default;

private:
    std::unordered_map<void*, AllocationInfo> allocations_;
    size_t totalAllocated_ = 0;
};

// Macro for tracking allocations
#define TRACKED_ALLOCATE(allocator, size, alignment) \
    [&]() { \
        void* ptr = allocator.Allocate(size, alignment); \
        TLETC::Memory::AllocationTracker::Get().RecordAllocation( \
            ptr, size, __FILE__, __LINE__, __FUNCTION__); \
        return ptr; \
    }()

#define TRACKED_FREE(allocator, ptr) \
    do { \
        TLETC::Memory::AllocationTracker::Get().RecordFree(ptr); \
        allocator.Free(ptr); \
    } while(0)

} // namespace TLETC::Memory