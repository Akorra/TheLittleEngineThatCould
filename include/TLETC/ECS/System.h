#pragma once

#include "TLETC/Core/Types.h"

namespace TLETC::ECS 
{
class Scene;

/**
 * System 
 * Base class
 * Update method called every frame
 */
class System
{
public:
    virtual ~System() = default;

    // Optional ordering inside same phase
    virtual int GetPriority() const { return 0; }

    virtual void Startup(Scene&) {}
    virtual void Shutdown(Scene&) {}

    virtual void PreUpdate(Scene&, float) {}
    virtual void FixedUpdate(Scene&, float) {} //< physics
    virtual void Update(Scene&, float) {}      //< input/animation/AI 
    virtual void PostUpdate(Scene&, float) {}

    virtual void PreRender(Scene&, float) {}
    virtual void Render(Scene&, float) {}
    virtual void PostRender(Scene&, float) {}
};

} // namespace TLETC::ECS