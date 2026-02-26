#pragma once

#include "TLETC/ECS/Systems/System.h"

namespace TLETC::ECS
{

struct Health
{
    float current = 100.0f;
    float max = 100.0f;
    
    bool IsDead() const { return current <= 0.0f; }
    float GetPercent() const { return current / max; }
};

class HealthSystem : public System
{
public:
    const char* Name() const override { return "HealthSystem"; }
    int Priority() const override { return 0; }
    
    void Startup(Scene& scene) override;
    void Update(Scene& scene, float dt) override;

private:
    std::vector<Entity> deadEntities_;
};

} // namespace TLETC::ECS