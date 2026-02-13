#pragma once

#include "TLETC/Core/Types.h"
#include <atomic>

namespace TLETC::Memory
{

struct MemoryStats
{
    std::atomic<size_t> totalAllocated{0};
    std::atomic<size_t> totalFreed{0};
    std::atomic<size_t> allocationCount{0};
    std::atomic<size_t> freeCount{0};
    
    size_t GetCurrentUsage() const 
    { 
        return totalAllocated.load() - totalFreed.load(); 
    }
    
    void RecordAllocation(size_t size)
    {
        totalAllocated += size;
        allocationCount++;
    }
    
    void RecordFree(size_t size)
    {
        totalFreed += size;
        freeCount++;
    }
    
    void Reset()
    {
        totalAllocated = 0;
        totalFreed = 0;
        allocationCount = 0;
        freeCount = 0;
    }
    
    void Print() const;
};

// Global stats (optional)
extern MemoryStats g_MemoryStats;

} // namespace TLETC::Memory