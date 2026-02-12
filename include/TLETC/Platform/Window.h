#pragma once

#include "TLETC/Core/Types.h"
#include <string>

struct GLFWwindow;

namespace TLETC 
{

// Forward declaration
class Input;

struct WindowProps
{
    std::string title = "[TLETC] The Little Engine That Could!";

    uint32 width    = 1280;
    uint32 height   = 720;
    bool vsync      = true;
    bool fullscreen = false;
};

/**
 * Window - Simple GLFW window wrapper
 */
class Window 
{
public:
    Window() = default;
    ~Window();
    
    bool Create(const WindowProps& props);
    void Destroy();
    
    void PollEvents();
    void SwapBuffers();
    
    bool ShouldClose() const;

    void SetInput(Input* input) { input_ = input; }

    uint32 GetWidth() const { return width_; }
    uint32 GetHeight() const { return height_; }
    float  GetAspectRatio() const { return static_cast<float>(width_) / static_cast<float>(height_); }
    
    GLFWwindow* GetNativeWindow() const { return window_; }

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
private:
    GLFWwindow* window_ = nullptr;
    Input*      input_  = nullptr;

    uint32      width_  = 0;
    uint32      height_ = 0;
    std::string title_;

    static int  instanceCount_;
};

} // namespace TLETC