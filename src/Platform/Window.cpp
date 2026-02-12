#include "TLETC/Platform/Window.h"
#include "TLETC/Platform/Input.h"
#include "TLETC/Core/Log.h"
#include "TLETC/Core/Assert.h"

#include <GLFW/glfw3.h>

namespace TLETC 
{

int Window::instanceCount_ = 0;

Window::~Window()
{
    Destroy();
}

bool Window::Create(const WindowProps& props) 
{
    width_  = props.width;
    height_ = props.height;
    title_  = props.title;

    // Initialize GLFW (only once)
    if (instanceCount_ == 0)
    {
        if (!glfwInit())
        {
            TLETC_ERROR("Failed to initialize GLFW");
            return false;
        }
        TLETC_INFO("GLFW initialized");
    }
    instanceCount_++;

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
    GLFWmonitor* monitor = props.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    window_ = glfwCreateWindow(width_, height_, title_.c_str(), monitor, nullptr);
    
    if (!window_)
    {
        TLETC_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(props.vsync ? 1 : 0);

    // Set callbacks
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    glfwSetScrollCallback(window_, ScrollCallback);
    
    TLETC_INFO("Window created: ", width_, "x", height_);

    return true;
}

void Window::Destroy() 
{
    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
        
        instanceCount_--;
        if (instanceCount_ == 0)
        {
            glfwTerminate();
            TLETC_INFO("GLFW terminated");
        }
    }
}

void Window::PollEvents() 
{ 
    glfwPollEvents(); 
}

void Window::SwapBuffers() 
{
    TLETC_ASSERT(window_, "Window is null!");
    if(window_)
        glfwSwapBuffers(window_);
}

bool Window::ShouldClose() const 
{
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->width_ = width;
    win->height_ = height;
    TLETC_INFO("Window resized: ", width, "x", height);
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
    self->input_->OnMouseButton(static_cast<MouseButton>(button), pressed);
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