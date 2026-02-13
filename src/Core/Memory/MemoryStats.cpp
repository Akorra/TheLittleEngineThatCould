#include "TLETC/Core/Memory/MemoryStats.h"
#include "TLETC/Core/Log.h"

namespace TLETC::Memory
{

MemoryStats g_MemoryStats;

void MemoryStats::Print() const
{
    TLETC_INFO("=== Memory Statistics ===");
    TLETC_INFO("Total Allocated: ", totalAllocated.load() / 1024, " KB");
    TLETC_INFO("Total Freed: ", totalFreed.load() / 1024, " KB");
    TLETC_INFO("Current Usage: ", GetCurrentUsage() / 1024, " KB");
    TLETC_INFO("Allocation Count: ", allocationCount.load());
    TLETC_INFO("Free Count: ", freeCount.load());
}

} // namespace TLETC::Memory
