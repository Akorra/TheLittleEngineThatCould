#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/Prefab.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Log.h"
#include "TLETC/ECS/ComponentDependencies.h"

#include <chrono>

using namespace TLETC;
using namespace TLETC::ECS;

struct MeshRenderer { /* ... */ };
struct Bounds       { /* ... */ };

// Example components
struct Transform
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
    
    Transform(vec3 pos = vec3(0), vec3 rot = vec3(0), vec3 scl = vec3(1))
        : position(pos), rotation(rot), scale(scl) {}
};

struct Velocity
{
    vec3 direction;
    float speed;
    
    Velocity(vec3 dir = vec3(0), float spd = 1.0f)
        : direction(dir), speed(spd) {}
};

struct Health
{
    int current;
    int max;
    
    Health(int hp = 100) : current(hp), max(hp) {}
};

struct Name
{
    std::string name;
    
    Name(const std::string& n = "Entity") : name(n) {}
};

// MeshRenderer requires Transform and Bounds
template<>
struct TLETC::ECS::ComponentDependencies<MeshRenderer>
{
    using Dependencies = std::tuple<Transform, Bounds>;
};

void TestBasicECS()
{
    TLETC_INFO("=== Testing Basic ECS ===");
    
    Scene scene;

    scene.OnComponentAdded<Transform>([](Entity e, Transform& t)
    {
        TLETC_INFO("Transform added to entity ", e.Index(), " at (", t.position.x, ", ", t.position.y, ")");    
    });
    
    Prefab playerPrefab;
    playerPrefab.AddComponent<Transform>(vec3(0));
    playerPrefab.AddComponent<Health>(100);
    playerPrefab.AddComponent<Velocity>(vec3(1,0,0), 5.0f);
    playerPrefab.AddComponent<Name>("Player");

    Prefab enemyPrefab;
    enemyPrefab.AddComponent<Transform>(vec3(10, 0, 0));
    enemyPrefab.AddComponent<Health>(50);
    enemyPrefab.AddComponent<Name>("Enemy");

    // Create entities
    Entity player = scene.Instantiate(playerPrefab);
    Entity enemy1 = scene.Instantiate(enemyPrefab);
    Entity enemy2 = scene.Instantiate(enemyPrefab);
    scene.AddComponent<Velocity>(enemy2, vec3(-1, 0, 0), 3.0f);
    
    TLETC_INFO("Created ", scene.GetAliveEntityCount(), " entities");
    
    // Query: All entities with Name component
    TLETC_INFO("\n--- All Named Entities ---");
    scene.View<Name>([&](Entity e, Name& name) {
        TLETC_INFO("Entity: ", name.name);
    });
    
    // Query: All entities with Transform + Velocity
    TLETC_INFO("\n--- Entities with Transform + Velocity ---");
    scene.View<Transform, Velocity>([&](Entity e, Transform& t, Velocity& v) {
        Name* name = scene.GetComponent<Name>(e);
        TLETC_INFO("Entity: ", name ? name->name : "Unknown",
                   " at (", t.position.x, ", ", t.position.y, ", ", t.position.z, ")",
                   " moving at speed ", v.speed);
    });
    
    // Update positions
    TLETC_INFO("\n--- Simulating Movement (dt = 0.1) ---");
    float dt = 0.1f;
    scene.View<Transform, Velocity>([&](Entity e, Transform& t, Velocity& v) {
        t.position += v.direction * v.speed * dt;
    });
    
    // Print new positions
    scene.View<Transform, Velocity>([&](Entity e, Transform& t, Velocity& v) {
        Name* name = scene.GetComponent<Name>(e);
        TLETC_INFO("Entity: ", name ? name->name : "Unknown",
                   " now at (", t.position.x, ", ", t.position.y, ", ", t.position.z, ")");
    });
}

void TestEntityReuse()
{
    TLETC_INFO("\n=== Testing Entity Reuse & Generations ===");
    
    Scene scene;
    
    // Create entity
    Entity original = scene.CreateEntity();
    scene.AddComponent<Name>(original, "Original");
    
    TLETC_INFO("Created entity: ID=", original.Index(), " Gen=", original.Generation());
    TLETC_INFO("Is valid: ", scene.IsValid(original) ? "YES" : "NO");
    
    // Destroy entity
    scene.DestroyEntity(original);
    TLETC_INFO("Destroyed entity");
    TLETC_INFO("Is valid: ", scene.IsValid(original) ? "YES" : "NO");
    
    // Create new entity (should reuse index but increment generation)
    Entity reused = scene.CreateEntity();
    scene.AddComponent<Name>(reused, "Reused");
    
    TLETC_INFO("Created new entity: ID=", reused.Index(), " Gen=", reused.Generation());
    
    // Try to access old entity (should fail!)
    Name* nameOld = scene.GetComponent<Name>(original);
    TLETC_INFO("Accessing old handle: ", nameOld ? nameOld->name : "NULL (correct!)");
    
    // Access new entity (should work)
    Name* nameNew = scene.GetComponent<Name>(reused);
    TLETC_INFO("Accessing new handle: ", nameNew ? nameNew->name : "NULL");
}

void TestPerformance()
{
    TLETC_INFO("\n=== Testing Performance ===");
    
    Scene scene;
    
    // Create 10,000 entities
    const size_t COUNT = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < COUNT; ++i)
    {
        Entity e = scene.CreateEntity();
        scene.AddComponent<Transform>(e, vec3(i, i, i));
        scene.AddComponent<Velocity>(e, vec3(1, 0, 0), 1.0f);
        
        if (i % 2 == 0)
            scene.AddComponent<Health>(e, 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto durationMili = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    TLETC_INFO("Created ", COUNT, " entities in ", durationMili.count(), " ms");
    TLETC_INFO("Alive entities: ", scene.GetAliveEntityCount());
    
    // Iteration performance
    start = std::chrono::high_resolution_clock::now();
    
    size_t count = 0;
    scene.View<Transform, Velocity>([&](Entity e, Transform& t, Velocity& v) {
        t.position += v.direction * 0.016f;
        count++;
    });
    
    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    TLETC_INFO("Updated ", count, " entities in ", duration.count(), " µs");
    
    // Sparse iteration (only half have Health)
    start = std::chrono::high_resolution_clock::now();
    
    count = 0;
    scene.View<Transform, Health>([&](Entity e, Transform& t, Health& h) {
        count++;
    });
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    TLETC_INFO("Sparse query found ", count, " entities in ", duration.count(), " µs");
}

void TestPrefabs()
{
    TLETC_INFO("\n=== Testing Prefabs ===");
    
    Scene scene;

    scene.OnComponentAdded<Name>([](Entity e, Name& t)
    {
        TLETC_INFO("Created Enemy ", e.Index(), " - ", t.name);    
    });

    Prefab enemyPrefab;
    enemyPrefab
        .AddComponent<Transform>(vec3(0))
        .AddComponent<Health>(50)
        .AddComponent<Velocity>(vec3(1, 0, 0), 3.0f)
        .AddComponent<Name>("Enemy");

    // Spawn one
    Entity enemy = scene.Instantiate(enemyPrefab);

// Spawn a wave of 100
scene.Instantiate(enemyPrefab, 3);
}

void TestDependencies()
{
    TLETC_INFO("\n=== Testing Component Dependencies ===");
    
    Scene scene;

    scene.OnComponentAdded<Bounds>([](Entity e, Bounds& b)
    {
        TLETC_INFO("Added Bounds to ", e.Index());    
    });

    scene.OnComponentAdded<Transform>([](Entity e, Transform& t)
    {
        TLETC_INFO("Added Transform to ", e.Index(), " at (", t.position.x, ", ", t.position.y, ")");    
    });

    Entity e = scene.CreateEntity();
    scene.AddComponent<MeshRenderer>(e);
}

int main()
{
    TestBasicECS();
    TestEntityReuse();
    TestPerformance();
    TestPrefabs();
    TestDependencies();
    
    return 0;
}