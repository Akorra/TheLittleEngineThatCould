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
    friend class Entity;
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
    Window& GetWindow()             { return *window_; }
    Input& GetInput()               { return *input_; }
    RenderDevice* GetRenderDevice() { return renderDevice_.get(); }

    // TODO: Multi camera support
    // Camera
    void SetCamera(class Camera* camera) { camera_ = camera; }
    class Camera* GetCamera() const { return camera_; }

    // Resource creation - Application manages lifetime
    class Material* CreateMaterial(const std::string& name = "Material");
    class Texture* CreateTexture();

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

    // Event control
    void SetEventsEnabled(bool enabled) { eventsEnabled_ = enabled; }
    bool AreEventsEnabled() const { return eventsEnabled_; }

    // Optional: Hook into input events at Application level (before behaviours)
    // Most code should use Behaviours, but this is available for special cases
    std::function<void(KeyCode, bool)>     OnKeyEvent;         // key, pressed
    std::function<void(MouseButton, bool)> OnMouseButtonEvent; // button, pressed
    std::function<void(Vec2, Vec2)>        OnMouseMoveEvent;   // position, delta
    std::function<void(Vec2)>              OnMouseScrollEvent; // offset

protected:
    // Game loop phases (in order)
    void ProcessInput();
    void EarlyUpdate();
    void Update();
    void LateUpdate();
    void PreRender();
    void Render();
    void PostRender();
    void ProcessDestroyQueue();  // Clean up deferred destructions

    // Cleanup resources
    void ShutdownResources(); 

    // TODO: REVIEW THIS!!!
    // Rendering helpers
    void RenderAllMeshRenderers();  // Collects and renders all MeshRenderer behaviours

    // Behaviour event management
    void RegisterBehaviourForEvents(Behaviour* behaviour);
    void UnregisterBehaviourFromEvents(Behaviour* behaviour);
    void RunBehaviourEvent(uint32 eventId, std::function<void(Behaviour*)> callback);

private:
    // Core systems
    UniquePtr<Window>       window_;
    UniquePtr<Input>        input_;
    UniquePtr<RenderDevice> renderDevice_;

    // Camera
    class Camera* camera_;

    // Resources (Application owns and manages)
    std::vector<UniquePtr<class Material>> materials_;
    std::vector<UniquePtr<class Texture>>  textures_;
    
    // Entities
    std::vector<UniquePtr<Entity>> entities_;
    std::vector<Entity*>           entitiesToDestroy_; // deferred destruction
    
    // Event-specific behaviour lists (much faster than iterating all entities!)
    // Maps event type -> list of behaviours that handle that event
    std::unordered_map<uint32, std::vector<Behaviour*>> behaviourEventLists_;
    std::unordered_map<uint32, bool>                    behaviourEventListsDirty_;  // Needs re-sorting
    
    // State
    bool running_;
    bool initialized_;
    bool eventsEnabled_;
    
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

// The locomotive pulls the whole train - powers the game loop
class Locomotive : public Application {
public:
    using Application::Application;
};

} // namespace TLETC