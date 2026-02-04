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
/**
 * Bit flags for behaviour events
 * Use these to specify which events your behaviour handles
 */
enum EventFlag : uint32 
{
    None              = 0,
    
    // Individual events
    EarlyUpdate       = 1 << 0,
    Update            = 1 << 1,
    LateUpdate        = 1 << 2,
    PreRender         = 1 << 3,
    Render            = 1 << 4,
    PostRender        = 1 << 5,
    KeyEvents         = 1 << 6,
    MouseButtonEvents = 1 << 7,
    MouseMoveEvents   = 1 << 8,
    MouseScrollEvents = 1 << 9,
    
    // Common combinations
    AllUpdate = EarlyUpdate | Update | LateUpdate,
    AllRender = PreRender | Render | PostRender,
    AllInput  = KeyEvents | MouseButtonEvents | MouseMoveEvents | MouseScrollEvents,
    All       = 0xFFFFFFFF
};
static const uint32 MaxEventFlags = 10; 

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

    // Execution Order ( Lower == Earliear )
    void  SetExecutionOrder(const uint16 priority) { executionOrder_ = priority; }
    const uint16 GetExecutionOrder() { return executionOrder_; }

    // usage : use single EventFlag as arg or a union of EventFlags. 
    // example - activate update events : ActivateEvents(EventFlag::EarlyUpdate | EventFlag::Update | EventFlag::LateUpdate); // unordered
    uint32 GetEvents() const                  { return eventFlags_; }
    void SetActiveEvents(const uint32 flags)  { eventFlags_ = flags; }  //< override events
    void ActivateEvents(const uint32 flags)   { eventFlags_ |= flags; }
    void DeactivateEvents(const uint32 flags) { eventFlags_ &= ~flags; }
    bool HasEvent(const uint32 flag) const    { return (eventFlags_ & flag) != 0;}
    void ClearEvents()                        { eventFlags_ = EventFlag::None; }

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
    uint16  executionOrder_;  
    uint32  eventFlags_;
};

// ============================================================================
// Railroad-Themed Aliases
// ============================================================================

// The telegraph sends signals between train components (dispatches events)
using Cargo = Behaviour;

} // namespace TLETC