#include "TLETC/Core/Application.h"
#include "TLETC/ECS/Systems/System.h"

using namespace TLETC;
using namespace TLETC::ECS;

// ================================================ Components ================================================

struct Transform { vec3 position; vec3 scale; bool dirty_=true; };
struct Velocity  { vec3 direction; float speed = 1.0f; };
struct DebugName { std::string value; };
struct Health    { float current = 100.0f; };
struct Lifetime  { float remaining; }; //< Component: entity destroys itself after N seconds

// ============================================================================================================
// ================================================= Systems ==================================================

// Movement system - updates entity's transform given a speed and direction
class MovementSystem : public System
{
public:
    MovementSystem() = default;

    const char* Name() const override { return "MovementSystem"; }

    void FixedUpdate(Scene& scene, float dt) override 
    {
        scene.View<Transform, Velocity>([dt](Entity e, Transform& t, Velocity& v) {
            t.position = t.position + v.direction * v.speed * dt; //< dt is interpolation alpha
            t.dirty_ = true;
        });
    }
};
 
// Cleanup system - destroys entities after x time
class LifetimeSystem : public System 
{
public:
    LifetimeSystem() = default;

    const char* Name() const override { return "LifetimeSystem"; }
    int Priority() const override     { return 1000; } // Run last

    void Update(Scene& scene, float dt) override
    {
        // Collect expired entities first (don't destroy while iterating!)
        std::vector<Entity> toDestroy;

        scene.View<Lifetime>([&](Entity e, Lifetime& lt)
        {
            lt.remaining -= dt;
            if (lt.remaining <= 0.0f)
                toDestroy.push_back(e);
        });

        for (Entity e : toDestroy)
            scene.DestroyEntity(e);
    }
};

class CombatSystem : public System
{
    void Update(Scene& scene, float dt) override
    {
        // Safe iteration - commands are deferred
        scene.View<Health>([&](Entity e, Health& h) {
            h.current -= 10;  // Take damage
            if (h.current <= 0)
            {
                commands_.DestroyEntity(e);  // Safe!
                TLETC_INFO("Entity died, will destroy after iteration");
            }
        });

        // Entities are actually destroyed when SystemManager calls FlushCommands
    }
};

// Debug system: prints all named entities
class DebugSystem : public System
{
public:
    const char* Name() const override { return "DebugSystem"; }
    int Priority() const override     { return 999; }

    void Update(Scene& scene, float dt) override
    {
        timer_ += dt;
        if (timer_ < 1.0f) return;  //< Print once per second
        timer_ = 0.0f;

        TLETC_INFO("--- Entity Report ---");
        scene.View<DebugName, Transform>([&](Entity e, DebugName& n, Transform& t) {
            TLETC_INFO("  ", n.value, " pos=(", t.position.x, ", ", t.position.y, ", ", t.position.z, ")");
        });
    }

private:
    float timer_ = 0.0f;
};

// ============================================================================================================
// =============================================== Application ================================================

class TestApp : public Application
{
public:
    TestApp() : Application("Example 04 - Systems") {}

    void OnStartup() override 
    {
        // Register systems (priority determines order)
        auto& world = GetWorld();
        world.AddSystem<MovementSystem>();
        world.AddSystem<LifetimeSystem>();
        world.AddSystem<DebugSystem>();
        world.AddSystem<CombatSystem>();

        // world.Startup() is called by Application::Run()
        // BEFORE OnStartup, so we need to restart or just 
        // add entities to the scene directly.

        auto& scene = GetScene();

        // Create player
        Entity player = scene.CreateEntity();
        scene.AddComponent<DebugName>(player, "Player");
        scene.AddComponent<Transform>(player, vec3(0.0f));
        scene.AddComponent<Velocity>(player, vec3(1, 0, 0), 2.0f);
        scene.AddComponent<Health>(player, 100.0f);

        // Create some bullets with lifetime
        for (int i = 0; i < 5; ++i)
        {
            Entity bullet = scene.CreateEntity();
            scene.AddComponent<DebugName>(bullet, "Bullet " + std::to_string(i));
            scene.AddComponent<Transform>(bullet, vec3(i * 2.0f, 0, 0));
            scene.AddComponent<Velocity>(bullet, vec3(0, 1, 0), 10.0f);
            scene.AddComponent<Lifetime>(bullet, 2.0f + i * 0.5f);
        }
    }

    void OnUpdate(float dt) override
    {
        // Close on Escape
        if (input_->IsKeyPressed(KeyCode::Escape))
            RequestShutdown();
    }
};

// ============================================================================================================

int main()
{
    TestApp app;
    app.Run();
    return 0;
}