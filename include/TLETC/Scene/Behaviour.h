#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Input.h"
#include "TLETC/Core/Event.h"

#include <string>

namespace TLETC 
{

// Forward declarations
class Entity;

/**
 * Behaviour - Base class for all game logic components
 * 
 * Behaviours are attached to entities and receive lifecycle callbacks.
 * Override the virtual methods to implement custom logic.
 */
class Behaviour 
{
public:
    Behaviour();
    virtual ~Behaviour();
    
    // Lifecycle callbacks
    virtual void OnInit() {}                    // Called once when behaviour is added
    virtual void OnDestroy() {}                 // Called when behaviour is removed
    
    // Update phase callbacks (in order)
    virtual void OnEarlyUpdate(float deltaTime) { (void)deltaTime; }  // Before main update
    virtual void OnUpdate(float deltaTime)      { (void)deltaTime; }  // Main game logic
    virtual void OnLateUpdate(float deltaTime)  { (void)deltaTime; }  // After main update
    
    // Rendering phase callbacks (in order)
    virtual void OnPreRender() {}               // Before rendering
    virtual void OnRender() {}                  // During rendering
    virtual void OnPostRender() {}              // After rendering
    
    // Input event callbacks
    virtual void OnKeyPressed(KeyCode key)  { (void)key; }
    virtual void OnKeyReleased(KeyCode key) { (void)key; }
    virtual void OnMouseButtonPressed(MouseButton button)   { (void)button; }
    virtual void OnMouseButtonReleased(MouseButton button)  { (void)button; }
    virtual void OnMouseMoved(const Vec2& position, const Vec2& delta) { (void)position; (void)delta; }
    virtual void OnMouseScrolled(const Vec2& offset) { (void)offset; }
    
    // Enable/disable
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }
    
    // Entity access
    Entity* GetEntity() const { return entity_; }
    
    // Name for debugging
    virtual const char* GetName() const { return "Behaviour"; }
    
protected:
    // Helper to get input (if entity has access to it)
    Input* GetInput() const;
    
private:
    friend class Entity;
    Entity* entity_;
    bool    enabled_;
};

} // namespace TLETC