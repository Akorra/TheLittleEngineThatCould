#pragma once

#include <cstdint>
#include <string>

namespace TLETC {

class Entity; // fwd

// Base class for all attachable components.
// Keep this lean: lifecycle + basic state + owner link.
class Component
{    
public:
    explicit Component(Entity* owner) : owner_(owner), enabled_(true), started_(false) {}

    virtual ~Component() = default;

    // ----- Identity / Debug -----
    virtual const char* GetTypeName() const { return "Component"; }

    // ----- Owner -----
    Entity* GetEntity() const { return owner_; }

    // ----- Enable / Disable -----
    bool IsEnabled() const { return enabled_; }
    void SetEnabled(bool enabled)
    {
        if (enabled_ == enabled) 
            return;
            
        enabled_ = enabled;
        if (enabled_) 
            OnEnable();
        else          
            OnDisable();
    }

    // ----- Lifecycle (called by Entity/Scene) -----
    // OnCreate: called immediately when constructed/attached.
    virtual void OnCreate() {}

    // OnStart: first frame this component becomes active (once).
    virtual void OnStart() {}

    // OnDestroy: before removal (unregister, release, etc.).
    virtual void OnDestroy() {}

    // OnEnable/OnDisable: state transitions.
    virtual void OnEnable() {}
    virtual void OnDisable() {}

    // ----- Frame hooks (OPTIONAL; typically in Behaviour) -----
    // These default to no-op to keep base lightweight.
    virtual void OnEarlyUpdate(float /*dt*/) {}
    virtual void OnUpdate(float /*dt*/) {}
    virtual void OnLateUpdate(float /*dt*/) {}
    virtual void OnPreRender() {}
    virtual void OnPostRender() {}

    // ----- Internal: managed by Entity/Scene -----
    // Ensures OnStart fires once when first active.
    void __EnsureStartedIfActive()
    {
        if (!started_ && enabled_)
        {
            started_ = true;
            OnStart();
        }
    }

protected:
    Entity* owner_;
    bool    enabled_;
    bool    started_;

};

}   // namespace TLETC