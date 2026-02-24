#pragma once

#include "TLETC/ECS/Systems/System.h"

namespace TLETC::ECS
{
/**
 * TransformSystem
 *
 * Rebuilds world matrices for all dirty transforms.
 * Processes hierarchy depth-first, roots first.
 *
 * Runs at high priority (early in FixedUpdate) so other
 * systems can rely on up-to-date world matrices.
 */
class TransformSystem : public System
{
public:
    const char* Name() const override { return "TransformSystem"; }
    int Priority()     const override { return -1000; }

    void FixedUpdate(Scene& scene, float dt) override;

private:
    void UpdateRecursive(Scene& scene, Entity entity, bool parentDirty);
};

} // namespace TLETC::ECS
