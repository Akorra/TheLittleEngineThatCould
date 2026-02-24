#pragma once

#include "TLETC/ECS/Systems/System.h"

namespace TLETC::ECS
{
/**
 * MovementSystem
 *
 * Applies velocity to transform every fixed step.
 * Lives in FixedUpdate for deterministic physics-ready movement.
 */
class MovementSystem : public System 
{
public:
    const char* Name() const override { return "MovementSystem"; }
    int Priority() const override     { return 0; }

    void FixedUpdate(Scene& scene, float dt) override;
};

} // namespace TLETC::ECS
