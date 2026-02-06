#include "TLETC/Core/Application.h"
#include "TLETC/Rendering/MeshRenderer.h"
#include "TLETC/Rendering/Camera.h"
#include "TLETC/Rendering/Texture.h"

#include "../../src/Platform/OpenGL/GLRenderDevice.h"

#include <iostream>
#include <algorithm>

namespace TLETC 
{

Application::Application(const std::string& title, uint32 width, uint32 height)
    : title_(title)
    , width_(width)
    , height_(height)
    , running_(false)
    , initialized_(false)
    , eventsEnabled_(true)
    , camera_(nullptr)
    , time_(0.0f)
    , deltaTime_(0.0f)
    , lastFrameTime_(0.0)
{
}

Application::~Application() 
{
    Shutdown();
}

bool Application::Initialize() 
{
    if (initialized_) return true;
    
    std::cout << "==========================================" << std::endl;
    std::cout << "  Initializing Application" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Create window
    window_ = MakeUnique<Window>();
    if (!window_->Create(width_, height_, title_)) 
    {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    std::cout << "Window created: " << width_ << "x" << height_ << std::endl;
    
    // Create render device
    renderDevice_ = MakeUnique<GLRenderDevice>();
    if (!renderDevice_->Initialize()) 
    {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }
    
    std::cout << "Renderer: " << renderDevice_->GetRendererName() << std::endl;
    std::cout << "OpenGL: "   << renderDevice_->GetAPIVersion() << std::endl;

    // Create input
    input_ = MakeUnique<Input>();
    input_->Initialize(window_->GetNativeWindow());
    
    std::cout << "Input system initialized" << std::endl;

    initialized_ = true;
    
    // Call user initialization
    OnInit();

    // Initialize all entities
    for (auto& entity : entities_)
        entity->Init();
    
    std::cout << "Application initialized successfully!" << std::endl;
    std::cout << std::endl;

    return true;
}

void Application::Run() 
{
    if (!initialized_) 
    {
        std::cerr << "Application not initialized! Call Initialize() first." << std::endl;
        return;
    }

    running_ = true;
    lastFrameTime_ = window_->GetTime();
    
    std::cout << "Starting main loop..." << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << std::endl;
    
    // Main game loop
    while (running_ && !window_->ShouldClose()) 
    {
        // Calculate delta time
        double currentTime = window_->GetTime();
        deltaTime_       = static_cast<float>(currentTime - lastFrameTime_);
        lastFrameTime_   = currentTime;
        time_            = static_cast<float>(currentTime);
        
        // Execute game loop phases IN ORDER
        ProcessInput();    // 1. Read hardware, fire input events
        EarlyUpdate();     // 2. Pre-physics, input handling
        Update();          // 3. Main game logic
        LateUpdate();      // 4. Post-logic, cameras, etc.
        PreRender();       // 5. Prepare for rendering
        Render();          // 6. Draw everything
        PostRender();      // 7. UI, debug overlays, cleanup

        // Process any deferred destructions (safe to destroy now)
        ProcessDestroyQueue();
        
        // Swap buffers
        window_->SwapBuffers();
    }

    std::cout << std::endl;
    std::cout << "Exiting main loop..." << std::endl;
}

void Application::Shutdown() 
{
    if (!initialized_) return;
    
    std::cout << "Shutting down application..." << std::endl;

    // Clean up resources BEFORE user shutdown (so user can still access them)
    // User shutdown called after for any custom cleanup
    
    // Call user shutdown
    OnShutdown();
    
    // Destroy all entities
    for (auto& entity : entities_)
        entity->Destroy();
    entities_.clear();

    // Clean up resources (materials and textures)
    ShutdownResources();
    
    // Shutdown systems
    if (input_) input_->Shutdown();
    if (renderDevice_) renderDevice_->Shutdown();
    if (window_) window_->Destroy();
    
    initialized_ = false;
    running_     = false;
    
    std::cout << "Shutdown complete!" << std::endl;
}

Entity* Application::CreateEntity(const std::string& name) 
{
    auto entity = MakeUnique<Entity>(name);
    entity->SetInput(input_.get());
    entity->SetApplication(this); // access to Application for event registration
    
    Entity* ptr = entity.get();
    entities_.push_back(std::move(entity));
    
    // Initialize if application is already running
    if (initialized_)
        ptr->Init();
    
    return ptr;
}

void Application::DestroyEntity(Entity* entity) 
{
    // Don't destroy immediately - queue for end of frame
    entitiesToDestroy_.push_back(entity);
}

Material* Application::CreateMaterial(const std::string& name) 
{
    auto material = MakeUnique<Material>(name);
    Material* ptr = material.get();
    materials_.push_back(std::move(material));
    return ptr;
}

Texture* Application::CreateTexture() 
{
    auto texture = MakeUnique<Texture>();
    Texture* ptr = texture.get();
    textures_.push_back(std::move(texture));
    return ptr;
}

// ============================================================================
// Game Loop Phases (IN ORDER)
// ============================================================================

void Application::ProcessInput() 
{
    // Poll window events
    window_->PollEvents();
    
    // Update input state (just reads hardware)
    input_->Update();

    /* if (input_->IsKeyJustPressed(KeyCode::Escape)) Close(); /* Turn on Handle ESC to close (built-in) */
    
    // Fire keyboard events - only for keys that changed
    for(KeyCode key : input_->GetKeysJustPressed())
    {
        // Optional Application-level callback
        if (OnKeyEvent) OnKeyEvent(key, true);
        
        // Run behaviours that handle key events
        RunBehaviourEvent(6, [key](Behaviour* b) { b->OnKeyPressed(key); });    
    }

    for (KeyCode key : input_->GetKeysJustReleased()) 
    {
        // Optional Application-level callback
        if (OnKeyEvent) OnKeyEvent(key, false);
        
        // Run behaviours that handle key events
        RunBehaviourEvent(6, [key](Behaviour* b) { b->OnKeyReleased(key); });
    }
    
    // Fire mouse button events - only for buttons that actually changed! (efficient!)
    for (MouseButton button : input_->GetMouseButtonsJustPressed()) 
    {
        // Optional Application-level callback
        if (OnMouseButtonEvent) OnMouseButtonEvent(button, true);
        
        // Run behaviours that handle mouse button events
        RunBehaviourEvent(7, [button](Behaviour* b) { b->OnMouseButtonPressed(button); });
    }
    
    for (MouseButton button : input_->GetMouseButtonsJustReleased()) 
    {
        // Optional Application-level callback
        if (OnMouseButtonEvent) OnMouseButtonEvent(button, false);
        
        // Run behaviours that handle mouse button events
        RunBehaviourEvent(7, [button](Behaviour* b) { b->OnMouseButtonReleased(button); });
    }
    
    // Fire mouse moved event
    Vec2 delta = input_->GetMouseDelta();
    if (delta.x != 0.0f || delta.y != 0.0f) 
    {
        Vec2 position = input_->GetMousePosition();

        // Optional Application-level callback
        if (OnMouseMoveEvent) OnMouseMoveEvent(position, delta);
        
        // Run behaviours that handle mouse move events
        RunBehaviourEvent(8, [position, delta](Behaviour* b) { b->OnMouseMoved(position, delta); });
    }
    
    // Fire mouse scrolled event
    Vec2 scroll = input_->GetMouseScroll();
    if (scroll.x != 0.0f || scroll.y != 0.0f) 
    {
        // Optional Application-level callback
        if (OnMouseScrollEvent) OnMouseScrollEvent(scroll);
        
        // Run behaviours that handle mouse scroll events
        RunBehaviourEvent(9, [scroll](Behaviour* b) { b->OnMouseScrolled(scroll); });
    }
    
    // Reset scroll after event is fired (scroll is per-frame)
    input_->ResetScrollDelta();
}

void Application::EarlyUpdate() 
{
    // Run behaviours that handle early update
    RunBehaviourEvent(0, [this](Behaviour* b) { b->OnEarlyUpdate(deltaTime_); });
}

void Application::Update() 
{
    // Run behaviours that handle update
    RunBehaviourEvent(1, [this](Behaviour* b) { b->OnUpdate(deltaTime_); });
    
    // Call user update
    OnUpdate(deltaTime_);
}

void Application::LateUpdate() 
{
    // Run behaviours that handle late update
    RunBehaviourEvent(2, [this](Behaviour* b) { b->OnLateUpdate(deltaTime_); });
}

void Application::PreRender() 
{
    // Run behaviours that handle pre-render
    RunBehaviourEvent(3, [](Behaviour* b) { b->OnPreRender(); });
}

// TODO: REVIEW - REMOVE USER RENDER ACCESS
void Application::Render() 
{
    // Run behaviours that handle render
    RunBehaviourEvent(4, [](Behaviour* b) { b->OnRender(); });

    // Render all MeshRenderers automatically (user can override OnRender to customize)
    RenderAllMeshRenderers();
    
    // Call user render
    OnRender();
}

void Application::PostRender() 
{
    // Run behaviours that handle post-render
    RunBehaviourEvent(5, [](Behaviour* b) { b->OnPostRender(); });
}

void Application::ProcessDestroyQueue() 
{
    if (entitiesToDestroy_.empty()) return;
    
    for (Entity* entity : entitiesToDestroy_) 
    {
        auto it = std::find_if(entities_.begin(), entities_.end(), [entity](const UniquePtr<Entity>& ptr) { return ptr.get() == entity; });
        
        if (it != entities_.end()) 
        {
            // Unregister all behaviours from event lists before destroying
            for (const auto& behaviour : (*it)->behaviours_) 
                UnregisterBehaviourFromEvents(behaviour.get());
            
            (*it)->Destroy();
            entities_.erase(it);
        }
    }
    
    entitiesToDestroy_.clear();
}

//TODO: add destroy queues
void Application::ShutdownResources() 
{
    std::cout << "Cleaning up resources..." << std::endl;
    
    // Destroy all textures (need RenderDevice)
    size_t textureCount = textures_.size();
    for (auto& texture : textures_) {
        if (texture->IsValid())
            texture->Destroy(renderDevice_.get());
    }
    textures_.clear();
    
    std::cout << "  Destroyed " << textureCount << " textures" << std::endl;
    
    // Materials are automatically deleted (unique_ptr)
    size_t materialCount = materials_.size();
    materials_.clear();
    
    std::cout << "  Deleted " << materialCount << " materials" << std::endl;
}

void Application::RenderAllMeshRenderers() {
    // Need a camera to render
    if (!camera_) return;
    
    // Collect all MeshRenderers from all entities
    std::vector<MeshRenderer*> renderers;
    for (auto& entity : entities_) {
        if (!entity->IsEnabled()) continue;
        
        // Get MeshRenderer behaviour if it has one
        auto* meshRenderer = entity->GetBehaviour<MeshRenderer>();
        if (meshRenderer && meshRenderer->GetMaterial()) {
            renderers.push_back(meshRenderer);
        }
    }
    
    // Sort by render queue (opaque first, then transparent)
    std::sort(renderers.begin(), renderers.end(), 
        [](MeshRenderer* a, MeshRenderer* b) {
            return a->GetMaterial()->GetRenderQueue() < b->GetMaterial()->GetRenderQueue();
        });
    
    // Get camera matrices
    Mat4 view = camera_->GetViewMatrix();
    Mat4 projection = camera_->GetProjectionMatrix(window_->GetAspectRatio());
    
    // Render all
    for (auto* renderer : renderers) {
        renderer->Render(renderDevice_.get(), view, projection);
    }
}

void Application::RegisterBehaviourForEvents(Behaviour* behaviour) 
{
    // Register behaviour for each event it handles
    for (uint32 i = 0; i < Behaviour::MaxEventFlags; ++i) 
    {  // We have 10 event types
        if (behaviour->HasEvent(1 << i)) 
        {
            behaviourEventLists_[i].push_back(behaviour);
            behaviourEventListsDirty_[i] = true;  // Needs sorting
        }
    }
}

void Application::UnregisterBehaviourFromEvents(Behaviour* behaviour) 
{
    // Remove behaviour from all event lists
    for (auto& pair : behaviourEventLists_) 
    {
        auto& list = pair.second;
        list.erase(std::remove(list.begin(), list.end(), behaviour), list.end());
    }
}

void Application::RunBehaviourEvent(uint32 eventId, std::function<void(Behaviour*)> callback) {
    // Check if events are enabled for certain event types
    if (!eventsEnabled_) 
    {
        // These events can be disabled (useful for pausing)
        switch (eventId) 
        {
        case 0:  // EarlyUpdate
        case 1:  // Update
        case 2:  // LateUpdate
        case 6:  // KeyEvents
        case 7:  // MouseButtonEvents
        case 8:  // MouseMoveEvents
        case 9:  // MouseScrollEvents
            return;
        default:
            break;
        }
    }
    
    // Sort if needed
    if (behaviourEventListsDirty_[eventId]) 
    {
        auto& list = behaviourEventLists_[eventId];
        std::sort(list.begin(), list.end(), [](Behaviour* a, Behaviour* b) { return a->GetExecutionOrder() < b->GetExecutionOrder(); });
        behaviourEventListsDirty_[eventId] = false;
    }
    
    // Run callbacks on behaviours that handle this event
    auto& behaviourList = behaviourEventLists_[eventId];
    size_t originalSize = behaviourList.size();
    
    for (auto* behaviour : behaviourList) 
    {
        if (behaviour->IsEnabled())
            callback(behaviour);
    }
    
    // If list size changed during iteration, something was added/removed
    // (shouldn't happen with deferred destruction, but check anyway)
    if (behaviourList.size() != originalSize)
        behaviourEventListsDirty_[eventId] = true;
}

} // namespace TLETC