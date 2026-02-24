#pragma once

#include "TLETC/Core/Types.h"

namespace TLETC::ECS
{

class Scene;

/**
 * System
 * 
 * Base class for all game systems.
 * 
 * Lifecycle: 
 *      Startup -> [PreUpdate -> FixedUpdate(N) -> Update -> PostUpdate → PreRender → Render → PostRender] × frames → Shutdown
 */

class System
{
    virtual ~System() = default;

    // Lifecycle
    virtual void Startup(Scene& scene)  {}
    virtual void Shutdown(Scene& scene) {}

    // Update Phases
    virtual void PreUpdate(Scene& scene, float dt)   {}
    virtual void FixedUpdate(Scene& scene, float dt) {}
    virtual void Update(Scene& scene, float dt)      {}
    virtual void PostUpdate(Scene& scene, float dt)  {}

    // Render Phases
    // alpha = interpolation factor [0, 1] between physics steps
    virtual void PreRender(Scene& scene, float alpha)  {}
    virtual void Render(Scene& scene, float alpha)     {}
    virtual void PostRender(Scene& scene, float alpha) {}
    
    // Order
    virtual int Priority() const { return 0; } //< lower runs first (default == 0)

    // Enable/Disable
    bool IsEnabled() const        { return enabled_; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    
private:
    bool enabled_ = true;
};

} // namespace TLETC::ECS
