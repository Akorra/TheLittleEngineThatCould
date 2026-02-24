#pragma once

#include "TLETC/Platform/Window.h"
#include "TLETC/Platform/Input.h"
#include "TLETC/ECS/Systems/SystemManager.h"

#include <string>

namespace TLETC
{

class Application
{
public:
    Application(const std::string& title="[TLETC] The Little Engine That Could!!!", uint32 width=1280, uint32 height=720);
    virtual ~Application();

    void Run();
    void RequestShutdown() { running_ = false; }

    // Access for subclasses
    ECS::SystemManager& GetWorld()  { return *world_; }
    ECS::Scene& GetScene()          { return world_->GetScene(); }
    Window& GetWindow()             { return *window_; }
    Input& GetInput()               { return *input_; }

protected:
    // [USER]: Override this in your solution
    virtual void OnStartup() {}
    virtual void OnShutdown() {}
    virtual void OnUpdate(float dt) { (void)dt; }
    virtual void OnRender() {}

private:
    bool Initialize();
    void Shutdown();

protected:
    UniquePtr<Window>             window_;
    UniquePtr<Input>              input_;
    UniquePtr<ECS::SystemManager> world_;

private:
    bool initialized_ = false;
    bool running_     = false;

    WindowProps windowProps_;
};

} // namespace TLETC
