#include <catch2/catch_test_macros.hpp>

#include "TLETC/Core/Memory/LinearAllocator.h"
#include "TLETC/Core/Memory/PoolAllocator.h"
#include "TLETC/Core/Memory/StackAllocator.h"
#include "TLETC/Core/Memory/MemoryStats.h"
#include "TLETC/Core/Containers/PackedArray.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Log.h"

using namespace TLETC;
using namespace TLETC::Memory;

struct Transform
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

TEST_CASE("Linear Allocator", "[memory][allocators]")
{
    LinearAllocator allocator(1024 * 1024); // 1 MB

    // Allocate some data
    int* numbers = static_cast<int*>(allocator.Allocate(sizeof(int) * 100));
    for (int i = 0; i < 100; ++i)
        numbers[i] = i;

    REQUIRE(allocator.GetAllocatedSize() == sizeof(int) * 100);

    float* floats = static_cast<float*>(allocator.Allocate(sizeof(float) * 50));
    for (int i = 0; i < 50; ++i)
        floats[i] = static_cast<float>(i) * 1.5f;

    REQUIRE(allocator.GetAllocatedSize() == sizeof(int) * 100 + sizeof(float) * 50);

    // Verify data
    REQUIRE(numbers[50] == 50);   //< Data corupted
    REQUIRE(floats[25] == 37.5f); //< Data corupted

    allocator.Reset();
    REQUIRE(allocator.GetAllocatedSize() == 0);
}

TEST_CASE("Pool Allocator", "[memory][allocators]")
{   
    PoolAllocator pool(sizeof(Transform), 1000);
    
    // Allocate transforms
    std::vector<Transform*> transforms;
    for (int i = 0; i < 500; ++i)
    {
        Transform* t = static_cast<Transform*>(pool.Allocate(sizeof(Transform)));
        t->position = vec3(i, i, i);
        transforms.push_back(t);
    }
    
    REQUIRE(pool.GetAllocatedChunks() == 500);
    REQUIRE(pool.GetFreeChunks() == 500);
    
    // Free half
    for (int i = 0; i < 250; ++i)
        pool.Free(transforms[i]);

    REQUIRE(pool.GetAllocatedChunks() == 250);
    REQUIRE(pool.GetFreeChunks() == 750);
    
    // Reallocate (should reuse freed chunks)
    for (int i = 0; i < 250; ++i)
    {
        Transform* t = static_cast<Transform*>(pool.Allocate(sizeof(Transform)));
        transforms[i] = t;
    }
    
    REQUIRE(pool.GetAllocatedChunks() == 500);
    REQUIRE(pool.GetFreeChunks() == 500);
}

TEST_CASE("Packed Array", "[memory][containers]")
{     
    // Create components  
    PackedArray<Transform> transforms;
    for (uint32 i = 0; i < 10000; ++i)
    {
        transforms.Emplace(i, 
            vec3(i, i, i),     // position
            vec3(0, 0, 0),     // rotation
            vec3(1, 1, 1)      // scale
        );
    }
    
    REQUIRE(transforms.Size() == 10000);
    
    REQUIRE(transforms.Get(5000)->position.x == 5000);

    for (uint32 i = 0; i < 10000; ++i)
    {
        Transform* t = transforms.Get(i);
        if (t)
            t->position.x += 1.0f;
    }

    REQUIRE(transforms.Get(5000)->position.x == 5001);

    // Remove test
    REQUIRE(transforms.Has(5000));

    transforms.Remove(5000);
    
    REQUIRE(!transforms.Has(5000)); // Remove failed!
    REQUIRE(transforms.Size() == 9999); // Size incorrect!
}