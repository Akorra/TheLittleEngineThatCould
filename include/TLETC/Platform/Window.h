#pragma once

#include "TLETC/Core/Input.h"
#include <string>

struct GLFWwindow;

namespace TLETC {

/**
 * Window - Simple GLFW window wrapper
 */
class Window {
public:
    Window();
    ~Window();
    
    bool Create(uint32 width, uint32 height, const std::string& title);
    void Destroy();
    
    void PollEvents();
    void SwapBuffers();
    
    bool ShouldClose() const;

    void SetInput(Input* input);

    uint32 GetWidth() const { return width_; }
    uint32 GetHeight() const { return height_; }
    float  GetAspectRatio() const { return static_cast<float>(width_) / static_cast<float>(height_); }
    
    GLFWwindow* GetNativeWindow() const { return window_; }

    double GetTime() const;

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
private:
    GLFWwindow* window_ = nullptr;
    Input*      input_  = nullptr;

    uint32      width_;
    uint32      height_;
    std::string title_;
};

} // namespace TLETC