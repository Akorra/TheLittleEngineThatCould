#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Platform/Window.h"
#include "TLETC/Platform/Input.h"

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
    UniquePtr<Window> window_;
    UniquePtr<Input>  input_;

private:
    bool initialized_ = false;
    bool running_     = false;

    WindowProps windowProps_;
};

} // namespace TLETC
