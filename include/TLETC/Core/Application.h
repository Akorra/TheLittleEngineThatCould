#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Window.h"
#include "TLETC/Core/Input.h"
#include "TLETC/Core/Event.h"
#include "TLETC/Core/EventDispatcher.h"
#include "TLETC/Scene/Entity.h"
#include "TLETC/Rendering/RenderDevice.h"

#include <vector>
#include <memory>

namespace TLETC 
{
/**
 * Application - Main game loop with ordered event phases
 * 
 * Manages the game loop and ensures events/updates happen in the correct order:
 * 1. Process Input (read hardware)
 * 2. Early Update (pre-physics, input handling)
 * 3. Update (main game logic)
 * 4. Late Update (post-logic, camera follow, etc.)
 * 5. Pre Render (prepare rendering)
 * 6. Render (draw)
 * 7. Post Render (cleanup, UI overlays)
 */
class Application 
{
public:
    Application(const std::string& title = "TLETC Application", uint32 width=1280, uint32 height=720);
    virtual ~Application();

    // Lifecycle
    bool Initialize();
    void Run();
    void Shutdown();

    // Override these for custom behavios
    virtual void OnInit() {}
    virtual void OnShutdown() {}
    virtual void OnUpdate(float deltaTime) { (void)deltaTime; }
    virtual void OnRender() {}

    // Access to core systems
    Window& GetWindow() { return *window_; }
    Input& GetInput()   { return *input_; }
    EventDispatcher& GetEventDispatcher() { return *eventDispatcher_; }
    RenderDevice* GetRenderDevice()       { return renderDevice_.get(); }

    // Entity management
    Entity* CreateEntity(const std::string& name = "Entity");
    void    DestroyEntity(Entity* entity);
    const std::vector<UniquePtr<Entity>>& GetEntities() const { return entities_; }

    // Control
    void Close()           { running_ = false; }
    bool IsRunning() const { return running_; }
    
    // Time
    float GetDeltaTime() const { return deltaTime_; }
    float GetTime() const      { return time_; }

protected:
    // Game loop phases (in order)
    void ProcessInput();
    void EarlyUpdate();
    void Update();
    void LateUpdate();
    void PreRender();
    void Render();
    void PostRender();

private:
    // Core systems
    UniquePtr<Window> window_;
    UniquePtr<Input>  input_;
    UniquePtr<EventDispatcher> eventDispatcher_;
    UniquePtr<RenderDevice>    renderDevice_;
    
    // Entities
    std::vector<UniquePtr<Entity>> entities_;
    
    // State
    bool running_;
    bool initialized_;
    
    // Time
    float  time_;
    float  deltaTime_;
    double lastFrameTime_;
    
    // Window properties
    std::string title_;
    uint32      width_;
    uint32      height_;
};

// Railroad-themed aliases for Application
// Use whichever name fits your style!

// "The Little Engine That Could" - the main engine that drives everything
using Engine = Application;

// The locomotive pulls the whole train - powers the game loop
using Locomotive = Application;

} // namespace TLETC