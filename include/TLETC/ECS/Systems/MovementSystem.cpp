#include "TLETC/ECS/Systems/MovementSystem.h"
#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Components/Velocity.h"

namespace TLETC::ECS
{
void MovementSystem::FixedUpdate(Scene &scene, float dt)
{
    scene.View<Transform, Velocity>([dt](Entity e, Transform& t, Velocity& v)
    {
        if (v.space_ == VelocitySpace::World)
            t.position_ += v.linear_ * dt; //< Apply linear velocity
        else  // Local space
            t.position_ += (t.rotation_ * v.linear_) * dt; //< Rotate velocity by parent's rotation

        // Apply angular velocity (rotation)
        if (length(v.angular_) > 0.001f)
        {
            quat angularRot = angleAxis(length(v.angular_) * dt, normalize(v.angular_));
            t.rotation_ = t.rotation_ * angularRot;
        }

        t.dirty_ = true;
    });
}

} // namespace TLETC::ECS

