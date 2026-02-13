// tests/MemoryTest.cpp
#include "TLETC/Core/Memory/LinearAllocator.h"
#include "TLETC/Core/Memory/PoolAllocator.h"
#include "TLETC/Core/Memory/StackAllocator.h"
#include "TLETC/Core/Memory/MemoryStats.h"
#include "TLETC/Core/Containers/PackedArray.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Log.h"

#include <chrono>

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

int main()
{
    TestLinearAllocator();
    TLETC_INFO("");
    
    TestPoolAllocator();
    TLETC_INFO("");
    
    TestPackedArray();
    TLETC_INFO("");
    
    g_MemoryStats.Print();
    
    return 0;
}