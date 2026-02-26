#include "TLETC/Core/Application.h"
#include "TLETC/ECS/Systems/TransformSystem.h"
#include "TLETC/ECS/Components/Transform.h"

#include "Systems/HealthSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/CombatSystem.h"

#include "Events/Events.h"

#include "TLETC/Core/Log.h"

using namespace TLETC;
using namespace TLETC::ECS;

struct DebugName { std::string value; };

class EventsTestApp : public Application
{
public:
    EventsTestApp() : Application("Milestone 6 - Events") {}
    
    void OnStartup() override
    {
        auto& world = GetWorld();
        
        // Add systems
        world.AddSystem<TransformSystem>();
        world.AddSystem<HealthSystem>();
        world.AddSystem<ScoreSystem>();
        combatSystem_ = &world.AddSystem<CombatSystem>();
        
        auto& scene = GetScene();
        
        // Create player
        player_ = scene.CreateEntity();
        scene.AddComponent<DebugName>(player_, "Player");
        scene.AddComponent<Transform>(player_, vec3(0, 0, 0));
        scene.AddComponent<Health>(player_);
        scene.AddComponent<Weapon>(player_);
        
        // Create enemy
        enemy_ = scene.CreateEntity();
        scene.AddComponent<DebugName>(enemy_, "Enemy");
        scene.AddComponent<Transform>(enemy_, vec3(5, 0, 0));
        scene.AddComponent<Health>(enemy_);
        
        TLETC_INFO("Press SPACE to fire at enemy");
        TLETC_INFO("Press H to heal player");
    }
    
    void OnUpdate(float dt) override
    {
        auto& scene = GetScene();
        
        // Fire weapon
        if (GetInput().IsKeyPressed(KeyCode::Space))
        {
            if (scene.IsValid(player_) && scene.IsValid(enemy_))
            {
                combatSystem_->Fire(scene, player_, enemy_);
            }
        }
        
        // Heal player
        if (GetInput().IsKeyPressed(KeyCode::H))
        {
            if (scene.IsValid(player_))
            {
                HealEvent heal;
                heal.target = player_;
                heal.amount = 25.0f;
                GetWorld().GetEventBus().Publish(heal);
            }
        }
        
        // Check score
        ScoreSystem* scoreSystem = GetWorld().GetSystem<ScoreSystem>();
        if (scoreSystem && GetInput().IsKeyPressed(KeyCode::S))
        {
            TLETC_INFO("Current score: ", scoreSystem->GetScore());
        }
        
        // Quit
        if (GetInput().IsKeyPressed(KeyCode::Escape))
            RequestShutdown();
    }

private:
    Entity player_;
    Entity enemy_;
    CombatSystem* combatSystem_ = nullptr;
};

int main()
{
    EventsTestApp app;
    app.Run();
    return 0;
}