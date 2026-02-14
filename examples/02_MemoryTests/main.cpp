// tests/MemoryTest.cpp
#include "TLETC/Core/Memory/LinearAllocator.h"
#include "TLETC/Core/Memory/PoolAllocator.h"
#include "TLETC/Core/Memory/StackAllocator.h"
#include "TLETC/Core/Memory/MemoryStats.h"
#include "TLETC/Core/Containers/PackedArray.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Log.h"

#include <chrono>
#include <vector>
#include <list>
#include <map>

using namespace TLETC;
using namespace TLETC::Memory;

struct Transform
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

void TestLinearAllocator()
{
    TLETC_INFO("=== Testing LinearAllocator ===");
    
    LinearAllocator allocator(1024 * 1024); // 1 MB
    
    // Allocate some data
    int* numbers = static_cast<int*>(allocator.Allocate(sizeof(int) * 100));
    for (int i = 0; i < 100; ++i)
        numbers[i] = i;
    
    float* floats = static_cast<float*>(allocator.Allocate(sizeof(float) * 50));
    for (int i = 0; i < 50; ++i)
        floats[i] = static_cast<float>(i) * 1.5f;
    
    TLETC_INFO("Allocated: ", allocator.GetAllocatedSize(), " bytes");
    
    // Verify data
    TLETC_ASSERT(numbers[50] == 50, "Data corrupted!");
    TLETC_ASSERT(floats[25] == 37.5f, "Data corrupted!");
    
    // Reset
    allocator.Reset();
    TLETC_INFO("After reset: ", allocator.GetAllocatedSize(), " bytes");
}

void TestDoubleEndedStack()
{
    TLETC_INFO("=== Testing DoubleEndedStackAllocator ===");
    
    DoubleEndedStackAllocator allocator(1024);
    
    // Allocate from bottom
    int* bottomData1 = static_cast<int*>(allocator.AllocateBottom(sizeof(int) * 10));
    for (int i = 0; i < 10; ++i) bottomData1[i] = i;
    
    // Allocate from top
    float* topData1 = static_cast<float*>(allocator.AllocateTop(sizeof(float) * 10));
    for (int i = 0; i < 10; ++i) topData1[i] = i * 1.5f;
    
    // More from bottom
    int* bottomData2 = static_cast<int*>(allocator.AllocateBottom(sizeof(int) * 10));
    
    // More from top
    float* topData2 = static_cast<float*>(allocator.AllocateTop(sizeof(float) * 10));
    
    TLETC_INFO("Free space between stacks: ", allocator.GetFreeSpace(), " bytes");
    TLETC_INFO("Total allocated: ", allocator.GetAllocatedSize(), " bytes");
    
    // Verify data
    TLETC_ASSERT(bottomData1[5] == 5, "Bottom data corrupted!");
    TLETC_ASSERT(topData1[5] == 7.5f, "Top data corrupted!");
    
    // Free in LIFO order
    allocator.FreeTop(topData2);
    allocator.FreeTop(topData1);
    allocator.FreeBottom(bottomData2);
    allocator.FreeBottom(bottomData1);
    
    TLETC_INFO("After freeing all: ", allocator.GetAllocatedSize(), " bytes");
}

void TestPoolAllocator()
{
    TLETC_INFO("=== Testing PoolAllocator ===");
    
    PoolAllocator pool(sizeof(Transform), 1000);
    
    // Allocate transforms
    std::vector<Transform*> transforms;
    for (int i = 0; i < 500; ++i)
    {
        Transform* t = static_cast<Transform*>(pool.Allocate(sizeof(Transform)));
        t->position = vec3(i, i, i);
        transforms.push_back(t);
    }
    
    TLETC_INFO("Allocated chunks: ", pool.GetAllocatedChunks());
    TLETC_INFO("Free chunks: ", pool.GetFreeChunks());
    
    // Free half
    for (int i = 0; i < 250; ++i)
    {
        pool.Free(transforms[i]);
    }
    
    TLETC_INFO("After freeing 250: ", pool.GetAllocatedChunks(), " allocated");
    
    // Reallocate (should reuse freed chunks)
    for (int i = 0; i < 250; ++i)
    {
        Transform* t = static_cast<Transform*>(pool.Allocate(sizeof(Transform)));
        transforms[i] = t;
    }
    
    TLETC_INFO("After reallocating: ", pool.GetAllocatedChunks(), " allocated");
}

void TestPackedArray()
{
    TLETC_INFO("=== Testing PackedArray ===");
    
    PackedArray<Transform> transforms;
    
    // Add components
    auto start = std::chrono::high_resolution_clock::now();
    
    for (uint32 i = 0; i < 10000; ++i)
    {
        transforms.Emplace(i, 
            vec3(i, i, i),     // position
            vec3(0, 0, 0),     // rotation
            vec3(1, 1, 1)      // scale
        );
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    TLETC_INFO("Added 10,000 components in ", duration.count(), " µs");
    TLETC_INFO("Size: ", transforms.Size());
    
    // Iteration test
    start = std::chrono::high_resolution_clock::now();
    
    vec3 sum(0);
    for (auto& t : transforms)
    {
        sum += t.position;
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    TLETC_INFO("Iterated 10,000 components in ", duration.count(), " µs");
    
    // Random access test
    start = std::chrono::high_resolution_clock::now();
    
    for (uint32 i = 0; i < 10000; ++i)
    {
        Transform* t = transforms.Get(i);
        if (t)
            t->position.x += 1.0f;
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    TLETC_INFO("Random access 10,000 times in ", duration.count(), " µs");
    
    // Remove test
    transforms.Remove(5000);
    TLETC_ASSERT(!transforms.Has(5000), "Remove failed!");
    TLETC_ASSERT(transforms.Size() == 9999, "Size incorrect!");
}

void TestSTLPoolAllocator()
{
    TLETC_INFO("=== Testing STL Pool Allocator ===");

    using namespace TLETC::Memory;
    
    // Create pool for Transform-sized chunks
    PoolAllocator pool(sizeof(Transform), 1000);
    
    // Use with std::vector
    {
        std::vector<Transform, STLPoolAllocator<Transform>> transforms{STLPoolAllocator<Transform>(pool)};
        
        for (int i = 0; i < 100; ++i)
        {
            transforms.emplace_back();
            transforms.back().position = vec3(i, i, i);
        }
        
        TLETC_INFO("Vector size: ", transforms.size());
        TLETC_INFO("Pool usage: ", pool.GetAllocatedChunks(), "/", pool.GetAllocatedChunks() + pool.GetFreeChunks());
    }
    
    // Pool chunks are automatically returned
    TLETC_INFO("After vector destruction: ", pool.GetAllocatedChunks(), " chunks allocated");
}


int main()
{
    TestLinearAllocator();
    TLETC_INFO("");
    
    TestDoubleEndedStack();
    TLETC_INFO("");

    TestPoolAllocator();
    TLETC_INFO("");

    TestSTLPoolAllocator();
    TLETC_INFO("");

    TestPackedArray();
    TLETC_INFO("");
    
    g_MemoryStats.Print();
    
    return 0;
}