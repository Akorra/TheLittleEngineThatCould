#include "TLETC/Core/Application.h"

#include "TLETC/Platform/Window.h"
#include "TLETC/Core/Time.h"

namespace TLETC
{

Application::Application(const std::string& title, uint32 width, uint32 height) 
    : initialized_(false)
    , running_(false)
    , title_(title)
    , width_(width)
    , height_(height)
{
}

Application::~Application() 
{
}

void Application::Run() 
{
    if(!Initialize()) return;

    running_ = true;

    // Main Game Loop
    while(running_ && !window_->ShouldClose())
    {
        Time::Update();
        float frameDt = Time::DeltaTime();

        input_->BeginFrame();  //< 1. Begin Frame

        window_->PollEvents(); //< 2. Poll OS events

        //< 3. Update Systems 
        // world_.Tick(frameDt);
        //< 4. Render
        // world_.Render();

        window_->SwapBuffers(); //< 5. Present
    }

    Shutdown();
}

bool Application::Initialize() 
{
    // Create Window
    window_ = MakeUnique<Window>();
    if(!window_->Create(width_, height_, title_))
        return false;

    input_ = MakeUnique<Input>();
    window_->SetInput(input_.get());

    initialized_ = true;
    return true;
}

void Application::Shutdown() 
{
    if(!initialized_) return;

    // shutdown systems
    window_->Destroy();
    window_.reset();
    input_.reset();

    initialized_ = false;
    running_     = false;
} 

} // namespace TLETC