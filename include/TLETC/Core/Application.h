#pragma once

#include "TLETC/Core/Types.h"

#include <string>

namespace TLETC 
{

/**
 * Application - Main game loop with ordered event phases
 */
class Application
{
public:
    Application(const std::string& title = "The Little Engine That Could", uint32 width=1280, uint32 height=720);
    virtual ~Application();

    // game loop
    void Run();

private:
    bool Initialize();    
    void Shutdown(); 

private:
    bool initialized_;
    bool running_;
    uint32 width_, height_;
    std::string title_;

    // Core/Platform systems
    UniquePtr<class Window> window_;
    UniquePtr<class Input>  input_;
};

} // namespace TLETC