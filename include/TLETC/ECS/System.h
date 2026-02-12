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

    virtual void OnCreate(class Scene&) {}
    virtual void OnDestroy(class Scene&) {}

    virtual void Update(class Scene&, float dt) {}
    virtual void Render(class Scene&) {}
};

} // namespace TLETC::ECS