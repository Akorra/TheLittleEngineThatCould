// TLETC/ECS/Systems/CameraFollowSystem.h
#pragma once

#include "TLETC/ECS/Systems/System.h"
#include "TLETC/Core/Math.h"
#include "TLETC/ECS/Components/Transform.h"

namespace TLETC::ECS
{

struct CameraFollow
{
    Entity target;
    vec3 offset {0, 2, -5};  // Behind and above
    float smoothSpeed = 5.0f;
};

class CameraFollowSystem : public System
{
public:
    const char* Name() const override { return "CameraFollowSystem"; }
    int Priority() const override { return 100; }  // After movement
    
    void Update(Scene& scene, float dt) override
    {
        scene.View<Transform, CameraFollow>([&](Entity e, Transform& t, CameraFollow& follow)
        {
            if (!scene.IsValid(follow.target))
                return;
            
            Transform* targetT = scene.GetComponent<Transform>(follow.target);
            if (!targetT)
                return;
            
            // Desired position: target + offset
            vec3 targetPos = targetT->GetWorldPosition();
            vec3 desiredPos = targetPos + follow.offset;
            
            // Smooth movement
            float alpha = clamp(1 - exp(-follow.smoothSpeed * dt), 0.0f, 1.0f);
            vec3 currentPos = t.GetWorldPosition();
            vec3 newPos = currentPos*(1-alpha) + desiredPos*alpha;
            t.SetWorldPosition(newPos, &scene);
            
            // Look at target
            t.LookAt(targetPos, vec3(0, 1, 0), scene);
        });
    }
};

} // namespace TLETC::ECS