#pragma once

#include "TLETC/ECS/Systems/System.h"

namespace TLETC::ECS
{

struct Weapon
{
    float damage = 10.0f;
    float fireRate = 1.0f;  // Shots per second
    float nextFireTime = 0.0f;
};

class CombatSystem : public System
{
public:
    const char* Name() const override { return "CombatSystem"; }
    int Priority() const override { return 50; }
    
    void Update(Scene& scene, float dt) override;
    
    void Fire(Scene& scene, Entity shooter, Entity target);
};

} // namespace TLETC::ECS