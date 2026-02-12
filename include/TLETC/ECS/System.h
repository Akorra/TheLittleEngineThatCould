#pragma once

namespace TLETC::ECS 
{

/**
 * System 
 * Base class
 * Update method called every frame
 */
class System
{
public:
    virtual ~System() = default;
    virtual void Update(float dt, class Scene& scene) = 0;
};

} // namespace TLETC::ECS