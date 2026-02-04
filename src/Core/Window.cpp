#include "TLETC/Core/Window.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace TLETC 
{

static bool s_glfwInitialized = false;
static int  s_windowCount     = 0;

Window::Window() : window_(nullptr), width_(0), height_(0) 
{ }

Window::~Window() 
{ Destroy(); }

bool Window::Create(uint32 width, uint32 height, const std::string& title) 
{
    width_  = width;
    height_ = height;
    title_  = title;

    if(!s_glfwInitialized)
    {
        if(!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        s_glfwInitialized = true;
    }

    // Set OpenGL version (4.6 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enable 4x MSAA (anti-aliasing)
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if (!window_) 
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync
    
    s_windowCount++;
    
    return true;
}

void Window::Destroy() 
{
    if (window_) 
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
        
        s_windowCount--;
        if (s_windowCount == 0 && s_glfwInitialized)
        {
            glfwTerminate();
            s_glfwInitialized = false;
        }
    }
}

bool Window::ShouldClose() const 
{
    return window_ && glfwWindowShouldClose(window_);
}

void Window::PollEvents() 
{ 
    glfwPollEvents(); 
}

void Window::SwapBuffers() 
{
    if (window_) 
        glfwSwapBuffers(window_);
}

double Window::GetTime() const 
{
    return glfwGetTime();
}
} // namespace TLETC