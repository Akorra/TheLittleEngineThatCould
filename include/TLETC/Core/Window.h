#pragma once

#include "TLETC/Core/Types.h"
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
    
    bool ShouldClose() const;
    void PollEvents();
    void SwapBuffers();
    
    uint32 GetWidth() const { return width_; }
    uint32 GetHeight() const { return height_; }
    float GetAspectRatio() const { return static_cast<float>(width_) / static_cast<float>(height_); }
    
    GLFWwindow* GetNativeWindow() const { return window_; }
    
    // Time
    double GetTime() const;
    
private:
    GLFWwindow* window_;
    uint32 width_;
    uint32 height_;
    std::string title_;
};

} // namespace TLETC
