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
public:
    virtual ~System() = default;

    // Lifecycle
    virtual void Startup(Scene& scene)  { (void)scene; }
    virtual void Shutdown(Scene& scene) { (void)scene; }

    // Update Phases
    virtual void PreUpdate(Scene& scene, float dt)   { (void)scene; (void)dt; }
    virtual void FixedUpdate(Scene& scene, float dt) { (void)scene; (void)dt; }
    virtual void Update(Scene& scene, float dt)      { (void)scene; (void)dt; }
    virtual void PostUpdate(Scene& scene, float dt)  { (void)scene; (void)dt; }

    // Render Phases
    // alpha = interpolation factor [0, 1] between physics steps
    virtual void PreRender(Scene& scene, float alpha)  { (void)scene; (void)alpha; }
    virtual void Render(Scene& scene, float alpha)     { (void)scene; (void)alpha; }
    virtual void PostRender(Scene& scene, float alpha) { (void)scene; (void)alpha; }
    
    // Order
    virtual int Priority() const { return 0; } //< lower runs first (default == 0)

    // Identity
    virtual const char* Name() const { return "System"; }

    // Enable/Disable
    bool IsEnabled() const        { return enabled_; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    
private:
    bool enabled_ = true;
};

} // namespace TLETC::ECS
