#include "TLETC/Core/Application.h"
#include "TLETC/Platform/Time.h"
#include "TLETC/Core/Log.h"

namespace TLETC
{

Application::Application(const std::string& title, uint32 width, uint32 height)
{
    windowProps_.title = title;
    windowProps_.width = width;
    windowProps_.height = height;
}

Application::~Application()
{
    Shutdown();
}

void Application::Run()
{
    if (!Initialize())
    {
        TLETC_ERROR("Failed to initialize application");
        return;
    }

    TLETC_INFO("Application starting main loop");
    running_ = true;

    world_->Startup();
    OnStartup();
    
    // Main loop
    while (running_ && !window_->ShouldClose())
    {
        Time::Update();
        float dt = Time::DeltaTime();
        
        input_->BeginFrame();
        window_->PollEvents();
        
        world_->Tick(dt); //< ECS systems update
        OnUpdate(dt);     //< App-level   update

        world_->Render(); //< ECS systems render
        OnRender();       //< App-level   render
        
        window_->SwapBuffers();
    }
    
    OnShutdown();
    world_->Shutdown();
    Shutdown();
}

bool Application::Initialize()
{
    TLETC_INFO("Initializing application");
    
    input_ = MakeUnique<Input>();   //< Create input
    window_ = MakeUnique<Window>(); //< Create window

    if (!window_->Create(windowProps_))
    {
        TLETC_ERROR("Failed to create window");
        return false;
    }
    
    window_->SetInput(input_.get());

    world_ = MakeUnique<ECS::SystemManager>();
    
    initialized_ = true;
    return true;
}

void Application::Shutdown()
{
    if (!initialized_)
        return;
    
    TLETC_INFO("Shutting down application");
    
    world_.reset();
    window_->Destroy();
    window_.reset();
    input_.reset();
    
    initialized_ = false;
    running_ = false;
}

} // namespace TLETC
