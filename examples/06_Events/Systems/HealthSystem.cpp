#include "HealthSystem.h"
#include "Events/Events.h"

#include "TLETC/ECS/Events/CommonEvents.h"
#include "TLETC/ECS/Scene.h"

namespace TLETC::ECS
{

void HealthSystem::Startup(Scene& scene)
{
    // Subscribe to damage events
    events_->Subscribe<DamageEvent>([&](const DamageEvent& e) {
        Health* health = scene.GetComponent<Health>(e.target);
        if (!health)
            return;
        
        health->current -= e.amount;
        
        TLETC_INFO("Entity took ", e.amount, " damage. HP: ", health->current);
        
        if (health->IsDead())
        {
            // Publish death event
            events_->PublishDeferred(EntityDiedEvent{e.target, e.source});
        }
    });
    
    // Subscribe to heal events
    events_->Subscribe<HealEvent>([&](const HealEvent& e) {
        Health* health = scene.GetComponent<Health>(e.target);
        if (!health)
            return;
        
        health->current = std::min(health->current + e.amount, health->max);
        
        TLETC_INFO("Entity healed ", e.amount, " HP. HP: ", health->current);
    });
}

void HealthSystem::Update(Scene& scene, float dt)
{
    // Collect dead entities
    deadEntities_.clear();
    
    scene.View<Health>([&](Entity e, Health& h) {
        if (h.IsDead())
            deadEntities_.push_back(e);
    });
    
    // Destroy dead entities (deferred)
    for (Entity e : deadEntities_)
    {
        scene.DestroyEntity(e);
    }
}

} // namespace TLETC::ECS