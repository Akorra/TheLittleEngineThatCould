#include "TLETC/Platform/Window.h"

#include <GLFW/glfw3.h>

namespace TLETC 
{

Window::Window() : window_(nullptr), input_(nullptr), width_(0), height_(0) 
{ 

}

Window::~Window() 
{ 
    Destroy(); 
    glfwTerminate();
}

bool Window::Create(uint32 width, uint32 height, const std::string& title) 
{
    width_  = width;
    height_ = height;
    title_  = title;

    if(!glfwInit())
        return false;

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
        return false;

    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    glfwSetScrollCallback(window_, ScrollCallback);

    glfwSwapInterval(1); // Enable vsync
    
    return true;
}

void Window::Destroy() 
{
    if (window_) 
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
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

bool Window::ShouldClose() const 
{
    return window_ && glfwWindowShouldClose(window_);
}

double Window::GetTime() const 
{
    return glfwGetTime();
}

void Window::SetInput(Input* input)
{
    input_ = input;
}


// Callbacks -------------------------------------------------- //
void Window::KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->input_)
        return;

    bool pressed = action != GLFW_RELEASE;
    self->input_->OnKeyEvent(static_cast<KeyCode>(key), pressed);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->input_)
        return;

    bool pressed = action != GLFW_RELEASE;
    self->input_->OnMouseButtonEvent(static_cast<MouseButton>(button), pressed);
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->input_)
        return;

    self->input_->OnMouseMove((float)xpos, (float)ypos);
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->input_)
        return;

    self->input_->OnMouseScroll((float)xoffset, (float)yoffset);
}

} // namespace TLETC