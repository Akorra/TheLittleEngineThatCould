#pragma once

#include "TLETC/ECS/System.h"
#include "TLETC/ECS/Entity.h"

namespace TLETC::ECS
{
class TransformSystem : public System
{
public:
    void FixedUpdate(Scene& scene, float dt) override;

private:
    void UpdateRecursive(Scene& scene, Entity e, bool parentDirty);
};
} // namespace TLETC::ECS
