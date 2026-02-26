#include "TLETC/Core/Application.h"
#include "TLETC/ECS/Systems/TransformSystem.h"
#include "TLETC/ECS/Systems/MovementSystem.h"
#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Components/Velocity.h"
#include "TLETC/Core/Log.h"

using namespace TLETC;
using namespace TLETC::ECS;

struct DebugName { std::string value; };

class DebugDrawSystem : public System
{
public:
    const char* Name() const override { return "DebugDrawSystem"; }
    int Priority()     const override { return 1000; }

    void Update(Scene& scene, float dt) override
    {
        timer_ += dt;
        if (timer_ < 1.0f) return;
        timer_ = 0.0f;

        TLETC_INFO("=== Transform Hierarchy ===");
        PrintHierarchy(scene, scene.GetRootEntities(), 0);
    }

private:
    void PrintHierarchy(Scene& scene, const std::vector<Entity>& entities, int depth)
    {
        std::string indent(depth * 2, ' ');

        for (Entity e : entities)
        {
            if (!scene.IsValid(e)) continue;

            DebugName* name = scene.GetComponent<DebugName>(e);
            Transform* t    = scene.GetComponent<Transform>(e);

            if (t)
            {
                vec3 wp = t->GetWorldPosition();
                TLETC_INFO(indent, name ? name->value : "Entity", 
                    " world=(", wp.x, ", ", wp.y, ", ", wp.z, ")", 
                    " local=(", t->position_.x, ", ", t->position_.y, ", ", t->position_.z, ")");
                PrintHierarchy(scene, t->children_, depth + 1);
            }
        }
    }

    float timer_ = 0.0f;
};

class HierarchyTest : public Application
{
public:
    HierarchyTest() : Application("Example 05 - Hierarchy") {}

    void OnStartup() override
    {
        auto& world = GetWorld();
        world.AddSystem<TransformSystem>();
        world.AddSystem<MovementSystem>();
        world.AddSystem<DebugDrawSystem>();

        auto& scene = GetScene();

        // Create a simple hierarchy:
        // Player
        //   ├─ Body
        //   │   └─ Head
        //   └─ Weapon

        Entity player = scene.CreateEntity();
        scene.AddComponent<DebugName>(player, "Player");
        scene.AddComponent<Transform>(player, vec3(0, 0, 0));
        scene.AddComponent<Velocity>(player, vec3(1, 0, 0), 1.0f);

        Entity body = scene.CreateEntity();
        scene.AddComponent<DebugName>(body, "Body");
        scene.AddComponent<Transform>(body, vec3(0, 1, 0));  // Above player
        scene.SetParent(body, player);

        Entity head = scene.CreateEntity();
        scene.AddComponent<DebugName>(head, "Head");
        scene.AddComponent<Transform>(head, vec3(0, 0.5f, 0));  // Above body
        scene.SetParent(head, body);

        Entity weapon = scene.CreateEntity();
        scene.AddComponent<DebugName>(weapon, "Weapon");
        scene.AddComponent<Transform>(weapon, vec3(1, 0.5f, 0));  // Right of player
        scene.SetParent(weapon, player);

        // Add spinning to body (local space angular velocity)
        scene.AddComponent<Velocity>(body, vec3(0, 0, 0), 0.0f);
        Velocity* bodyVel = scene.GetComponent<Velocity>(body);
        bodyVel->angular_ = vec3(0, 1, 0);  // Spin around Y axis
        bodyVel->space_   = VelocitySpace::Local;

        TLETC_INFO("Created hierarchy with ", scene.GetAliveEntityCount(), " entities");
    }

    void OnUpdate(float dt) override
    {
        if (input_->IsKeyDown(KeyCode::Escape))
            RequestShutdown();

        // Test: destroy player (should orphan children)
        if (input_->IsKeyDown(KeyCode::D))
        {
            auto& scene = GetScene();
            auto roots = scene.GetRootEntities();
            if (!roots.empty())
            {
                TLETC_INFO("Destroying root entity...");
                scene.DestroyEntity(roots[0]);
            }
        }

        // Test: recursive destroy
        if (input_->IsKeyDown(KeyCode::R))
        {
            auto& scene = GetScene();
            auto roots = scene.GetRootEntities();
            if (!roots.empty())
            {
                TLETC_INFO("Recursively destroying root entity and children...");
                scene.DestroyEntityRecursive(roots[0]);
            }
        }
    }
};

int main()
{
    HierarchyTest app;
    app.Run();
    return 0;
}
