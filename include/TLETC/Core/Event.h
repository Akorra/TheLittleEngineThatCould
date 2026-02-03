#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Input.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace TLETC 
{
// Event Categories for Filtering
enum class EventCategory 
{
    None        = 0,
    Input       = 1 << 0,
    Keyboard    = 1 << 1,
    Mouse       = 1 << 2,
    Application = 1 << 3,
    Window      = 1 << 4
};

// Base Event Class
class Event 
{
public:
    virtual ~Event() = default;

    bool handled = false;

    virtual const char* GetName() const = 0;
    virtual EventCategory GetCategory() const = 0;
};

// Keyboard Events
class KeyPressedEvent : public Event
{
public:
    KeyCode key;
    bool    repeat;

    KeyPressedEvent(KeyCode k, bool r = false) : key(k), repeat(r) {}

    const char* GetName() const override { return "KeyPressed"; }
    EventCategory GetCategory() const override { return EventCategory::Keyboard; }
};

class KeyReleasedEvent : public Event 
{
public:
    KeyCode key;
    
    KeyReleasedEvent(KeyCode k) : key(k) {}
    
    const char* GetName() const override { return "KeyReleased"; }
    EventCategory GetCategory() const override { return EventCategory::Keyboard; }
};

// Mouse button events
class MouseButtonPressedEvent : public Event 
{
public:
    MouseButton button;
    
    MouseButtonPressedEvent(MouseButton b) : button(b) {}
    
    const char* GetName() const override { return "MouseButtonPressed"; }
    EventCategory GetCategory() const override { return EventCategory::Mouse; }
};

class MouseButtonReleasedEvent : public Event 
{
public:
    MouseButton button;
    
    MouseButtonReleasedEvent(MouseButton b) : button(b) {}
    
    const char* GetName() const override { return "MouseButtonReleased"; }
    EventCategory GetCategory() const override { return EventCategory::Mouse; }
};

class MouseMovedEvent : public Event 
{
public:
    Vec2 position;
    Vec2 delta;
    
    MouseMovedEvent(const Vec2& pos, const Vec2& d) : position(pos), delta(d) {}
    
    const char* GetName() const override { return "MouseMoved"; }
    EventCategory GetCategory() const override { return EventCategory::Mouse; }
};

class MouseScrolledEvent : public Event 
{
public:
    Vec2 offset;
    
    MouseScrolledEvent(const Vec2& o) : offset(o) {}
    
    const char* GetName() const override { return "MouseScrolled"; }
    EventCategory GetCategory() const override { return EventCategory::Mouse; }
};

// Window events
class WindowResizeEvent : public Event 
{
public:
    uint32 width;
    uint32 height;
    
    WindowResizeEvent(uint32 w, uint32 h) : width(w), height(h) {}
    
    const char* GetName() const override { return "WindowResize"; }
    EventCategory GetCategory() const override { return EventCategory::Window; }
};

class WindowCloseEvent : public Event 
{
public:
    WindowCloseEvent() {}
    
    const char* GetName() const override { return "WindowClose"; }
    EventCategory GetCategory() const override { return EventCategory::Window; }
};

// Application update phase events (in execution order)
class AppEarlyUpdateEvent : public Event 
{
public:
    float deltaTime;
    AppEarlyUpdateEvent(float dt) : deltaTime(dt) {}
    const char* GetName() const override { return "AppEarlyUpdate"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

class AppUpdateEvent : public Event 
{
public:
    float deltaTime;
    AppUpdateEvent(float dt) : deltaTime(dt) {}
    const char* GetName() const override { return "AppUpdate"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

class AppLateUpdateEvent : public Event 
{
public:
    float deltaTime;
    AppLateUpdateEvent(float dt) : deltaTime(dt) {}
    const char* GetName() const override { return "AppLateUpdate"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

class AppPreRenderEvent : public Event 
{
public:
    AppPreRenderEvent() {}
    const char* GetName() const override { return "AppPreRender"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

class AppRenderEvent : public Event 
{
public:
    AppRenderEvent() {}
    const char* GetName() const override { return "AppRender"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

class AppPostRenderEvent : public Event 
{
public:
    AppPostRenderEvent() {}
    const char* GetName() const override { return "AppPostRender"; }
    EventCategory GetCategory() const override { return EventCategory::Application; }
};

} //namespace TLETC