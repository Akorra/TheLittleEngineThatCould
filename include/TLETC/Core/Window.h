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
    
    uint32 GetWidth() const { return m_width; }
    uint32 GetHeight() const { return m_height; }
    float GetAspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }
    
    GLFWwindow* GetNativeWindow() const { return m_window; }
    
    // Time
    double GetTime() const;
    
private:
    GLFWwindow* m_window;
    uint32 m_width;
    uint32 m_height;
    std::string m_title;
};

} // namespace TLETC
