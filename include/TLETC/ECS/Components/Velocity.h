#pragma once

#include "TLETC/Core/Math.h"

namespace TLETC::ECS
{
enum class VelocitySpace
{
    World, //< Velocity in world coordinates
    Local  //< Velocity in local (parent) coordinates
};

struct Velocity 
{
    vec3 linear_{0.0f};
    vec3 angular_{0.0f}; //< Rotation velocity (euler angles per second)

    VelocitySpace space_ = VelocitySpace::World;

    Velocity() = default;
    Velocity(const vec3& lin, float speed = 1.0f, VelocitySpace s = VelocitySpace::World) : linear_(lin * speed), space_(s) {}
};

} // namespace TLETC::ECS
