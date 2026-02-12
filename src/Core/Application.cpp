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

    OnStartup();
    
    // Main loop
    while (running_ && !window_->ShouldClose())
    {
        Time::Update();
        float dt = Time::DeltaTime();
        
        input_->BeginFrame();
        window_->PollEvents();
        
        OnUpdate(dt);
        OnRender();
        
        window_->SwapBuffers();
    }
    
    OnShutdown();
    Shutdown();
}

bool Application::Initialize()
{
    TLETC_INFO("Initializing application");
    
    // Create input
    input_ = MakeUnique<Input>();
    
    // Create window
    window_ = MakeUnique<Window>();
    if (!window_->Create(windowProps_))
    {
        TLETC_ERROR("Failed to create window");
        return false;
    }
    
    window_->SetInput(input_.get());
    
    initialized_ = true;
    return true;
}

void Application::Shutdown()
{
    if (!initialized_)
        return;
    
    TLETC_INFO("Shutting down application");
    
    window_->Destroy();
    window_.reset();
    input_.reset();
    
    initialized_ = false;
    running_ = false;
}

} // namespace TLETC
