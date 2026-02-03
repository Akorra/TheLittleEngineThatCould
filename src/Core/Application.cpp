#include "TLETC/Core/Application.h"

#include "../../src/Platform/OpenGL/GLRenderDevice.h"

#include <iostream>

namespace TLETC 
{

Application::Application(const std::string& title, uint32 width, uint32 height)
    : title_(title)
    , width_(width), height_(height)
    , running_(false), initialized_(false)
    , time_(0.0f), deltaTime_(0.0f)
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
    
    // Create event dispatcher
    eventDispatcher_ = MakeUnique<EventDispatcher>();
    
    std::cout << "Input system initialized" << std::endl;

    // Subscribe to window close event
    eventDispatcher_->Subscribe<WindowCloseEvent>([this](WindowCloseEvent& e) { (void)e; running_ = false; });
    
    // Subscribe to key press for ESC to close
    eventDispatcher_->Subscribe<KeyPressedEvent>([this](KeyPressedEvent& e) {
        if (e.key == KeyCode::Escape) 
            Close();
    });

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
        
        // Swap buffers
        window_->SwapBuffers();
    }

    std::cout << std::endl;
    std::cout << "Exiting main loop..." << std::endl;
}

void Application::Shutdown() {
    if (!initialized_) return;
    
    std::cout << "Shutting down application..." << std::endl;
    
    // Call user shutdown
    OnShutdown();
    
    // Destroy all entities
    for (auto& entity : entities_)
        entity->Destroy();
    entities_.clear();
    
    // Shutdown systems
    if (input_) 
        input_->Shutdown();
    if (renderDevice_) 
        renderDevice_->Shutdown();
    if (window_) 
        window_->Destroy();
    
    initialized_ = false;
    running_     = false;
    
    std::cout << "Shutdown complete!" << std::endl;
}

Entity* Application::CreateEntity(const std::string& name) 
{
    auto entity = MakeUnique<Entity>(name);
    entity->SetInput(input_.get());
    
    Entity* ptr = entity.get();
    entities_.push_back(std::move(entity));
    
    // Initialize if application is already running
    if (initialized_)
        ptr->Init();
    
    return ptr;
}

void Application::DestroyEntity(Entity* entity) 
{
    auto it = std::find_if(entities_.begin(), entities_.end(),
        [entity](const UniquePtr<Entity>& ptr) {
            return ptr.get() == entity;
        });
    
    if (it != entities_.end()) 
    {
        (*it)->Destroy();
        entities_.erase(it);
    }
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
    
    // Now fire events based on state changes
    // Application owns event logic, Input just tracks state
    
    // Fire keyboard events
    for (uint16 i{0}; i < Input::MAX_KEYS; ++i) 
    {
        if (input_->IsKeyJustPressed(static_cast<KeyCode>(i))) 
        {
            KeyPressedEvent event(static_cast<KeyCode>(i), false);
            eventDispatcher_->Dispatch(event);
        }
        if (input_->IsKeyJustReleased(static_cast<KeyCode>(i))) 
        {
            KeyReleasedEvent event(static_cast<KeyCode>(i));
            eventDispatcher_->Dispatch(event);
        }
    }
    
    // Fire mouse button events
    for (uint8 i{0}; i < Input::MAX_MOUSE_BUTTONS; ++i) 
    {
        if (input_->IsMouseButtonJustPressed(static_cast<MouseButton>(i))) 
        {
            MouseButtonPressedEvent event(static_cast<MouseButton>(i));
            eventDispatcher_->Dispatch(event);
        }
        if (input_->IsMouseButtonJustReleased(static_cast<MouseButton>(i))) 
        {
            MouseButtonReleasedEvent event(static_cast<MouseButton>(i));
            eventDispatcher_->Dispatch(event);
        }
    }
    
    // Fire mouse moved event
    Vec2 delta = input_->GetMouseDelta();
    if (delta.x != 0.0f || delta.y != 0.0f) 
    {
        MouseMovedEvent event(input_->GetMousePosition(), delta);
        eventDispatcher_->Dispatch(event);
    }
    
    // Fire mouse scrolled event
    Vec2 scroll = input_->GetMouseScroll();
    if (scroll.x != 0.0f || scroll.y != 0.0f) 
    {
        MouseScrolledEvent event(scroll);
        eventDispatcher_->Dispatch(event);
    }
    
    // Reset scroll after event is fired (scroll is per-frame)
    input_->ResetScrollDelta();
}

void Application::EarlyUpdate() 
{
    // Fire early update event
    AppEarlyUpdateEvent event(deltaTime_);
    eventDispatcher_->Dispatch(event);
    
    // Update entities - early phase
    for (auto& entity : entities_)
        entity->EarlyUpdate(deltaTime_);
}

void Application::Update() 
{
    // Fire update event
    AppUpdateEvent event(deltaTime_);
    eventDispatcher_->Dispatch(event);
    
    // Update entities - main phase
    for (auto& entity : entities_)
        entity->Update(deltaTime_);
    
    // Call user update
    OnUpdate(deltaTime_);
}

void Application::LateUpdate() 
{
    // Fire late update event
    AppLateUpdateEvent event(deltaTime_);
    eventDispatcher_->Dispatch(event);
    
    // Update entities - late phase
    for (auto& entity : entities_) 
        entity->LateUpdate(deltaTime_);
}

void Application::PreRender() 
{
    // Fire pre-render event
    AppPreRenderEvent event;
    eventDispatcher_->Dispatch(event);
    
    // Pre-render entities
    for (auto& entity : entities_)
        entity->PreRender();
}

void Application::Render() 
{
    // Fire render event
    AppRenderEvent event;
    eventDispatcher_->Dispatch(event);
    
    // Render entities
    for (auto& entity : entities_)
        entity->Render();
    
    // Call user render
    OnRender();
}

void Application::PostRender() 
{
    // Post-render entities
    for (auto& entity : entities_)
        entity->PostRender();
    
    // Fire post-render event
    AppPostRenderEvent event;
    eventDispatcher_->Dispatch(event);
}

} // namespace TLETC