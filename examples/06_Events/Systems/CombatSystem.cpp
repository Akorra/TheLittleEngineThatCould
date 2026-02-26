#include "CombatSystem.h"
#include "Events/Events.h"

#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Events/CommonEvents.h"
#include "TLETC/ECS/Scene.h"
#include "TLETC/Platform/Time.h"

namespace TLETC::ECS
{

void CombatSystem::Update(Scene& scene, float dt)
{
    // Example: Auto-fire at enemies
    // In real game, this would be triggered by player input
}

void CombatSystem::Fire(Scene& scene, Entity shooter, Entity target)
{
    Weapon* weapon = scene.GetComponent<Weapon>(shooter);
    Transform* shooterT = scene.GetComponent<Transform>(shooter);
    Transform* targetT = scene.GetComponent<Transform>(target);
    
    if (!weapon || !shooterT || !targetT)
        return;
    
    // Check fire rate
    float currentTime = Time::TotalTime();
    if (currentTime < weapon->nextFireTime)
        return;
    
    weapon->nextFireTime = currentTime + (1.0f / weapon->fireRate);
    
    // Publish damage event
    DamageEvent damage;
    damage.target = target;
    damage.source = shooter;
    damage.amount = weapon->damage;
    damage.position = targetT->GetWorldPosition();
    damage.direction = normalize(targetT->GetWorldPosition() - shooterT->GetWorldPosition());
    
    events_->Publish(damage);
    
    TLETC_INFO("Fired weapon! Damage: ", weapon->damage);
}

} // namespace TLETC::ECS