#include "TLETC/Core/Input.h"

#include <GLFW/glfw3.h>
#include <cstring>

namespace TLETC 
{
// static scroll callback pointer
static Input* s_inputInstance = nullptr;

// scroll callback
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(s_inputInstance)
    {
        s_inputInstance->mouseScroll_.x = static_cast<float>(xoffset);
        s_inputInstance->mouseScroll_.y = static_cast<float>(yoffset);
    }
}

Input::Input() : 
    window_(nullptr), 
    mousePosition_(0.0f), mousePositionLastFrame_(0.0f), 
    mouseDelta_(0.0f), mouseScroll_(0.0f), 
    cursorVisible_(true), cursorLocked_(false)
{
    std::memset(keys_, 0, sizeof(keys_));
    std::memset(keysLastFrame_, 0, sizeof(keysLastFrame_));
    std::memset(mouseButtons_, 0, sizeof(mouseButtons_));
    std::memset(mouseButtonsLastFrame_, 0, sizeof(mouseButtonsLastFrame_));
}

Input::~Input() 
{
    Shutdown();
}

void Input::Initialize(GLFWwindow* window)
{
    window_ = window;
    s_inputInstance = this;

    // Setup scroll callback
    glfwSetScrollCallback(window_, ScrollCallback);

    //Get initial mouse position
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    mousePosition_ = Vec2(static_cast<float>(x), static_cast<float>(y));
    mousePositionLastFrame_ = mousePosition_;
}

void Input::Shutdown()
{
    if(window_ && s_inputInstance == this)
    {
        glfwSetScrollCallback(window_, nullptr);
        s_inputInstance = nullptr;
    }
    window_ = nullptr;
}

void Input::Update()
{
    if(!window_) return;

    // Update Key State
    std::memcpy(keysLastFrame_, keys_, sizeof(keys_));
    for(uint16 i{0}; i<MAX_KEYS; ++i)
        keys_[i] = glfwGetKey(window_, i) == GLFW_PRESS;
    
    // Update mouse button state
    std::memcpy(mouseButtonsLastFrame_, mouseButtons_, sizeof(mouseButtons_));
    for(uint8 i{0}; i< MAX_MOUSE_BUTTONS; ++i)
        mouseButtons_[i] = glfwGetMouseButton(window_, i) == GLFW_PRESS;

    // Update Mouse Position
    mousePositionLastFrame_ = mousePosition_;
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    mousePosition_ = Vec2(static_cast<float>(x), static_cast<float>(y));
    mouseDelta_    = mousePosition_ - mousePositionLastFrame_;
}

bool Input::IsKeyPressed(const KeyCode& key) const 
{
    uint16 keyCode = static_cast<uint16>(key);
    if(keyCode < 0 || keyCode >= MAX_KEYS) return false;
    return keys_[keyCode];
}

bool Input::IsKeyJustPressed(const KeyCode& key) const 
{
    uint16 keyCode = static_cast<uint16>(key);
    if(keyCode < 0 || keyCode >= MAX_KEYS) return false;
    return keys_[keyCode] && !keysLastFrame_[keyCode];
}

bool Input::IsKeyJustReleased(const KeyCode& key) const 
{
    uint16 keyCode = static_cast<uint16>(key);
    if(keyCode < 0 || keyCode >= MAX_KEYS) return false;
    return !keys_[keyCode] && keysLastFrame_[keyCode];
}

bool Input::IsMouseButtonPressed(const MouseButton& button) const 
{
    uint8 btn = static_cast<uint8>(button);
    if (btn < 0 || btn >= MAX_MOUSE_BUTTONS) return false;
    return mouseButtons_[btn];
}

bool Input::IsMouseButtonJustPressed(const MouseButton& button) const 
{
    uint8 btn = static_cast<uint8>(button);
    if (btn < 0 || btn >= MAX_MOUSE_BUTTONS) return false;
    return mouseButtons_[btn] && !mouseButtonsLastFrame_[btn];
}

bool Input::IsMouseButtonJustReleased(const MouseButton& button) const 
{
    uint8 btn = static_cast<uint8>(button);
    if (btn < 0 || btn >= MAX_MOUSE_BUTTONS) return false;
    return !mouseButtons_[btn] && mouseButtonsLastFrame_[btn];
}

void Input::SetCursorVisible(bool visible) 
{
    if (!window_) return;
    cursorVisible_ = visible;
    
    if (visible)
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::SetCursorLocked(bool locked) 
{
    if (!window_) return;
    cursorLocked_ = locked;
    
    if (locked) 
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // Reset delta to avoid jump
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        mousePosition_ = Vec2(static_cast<float>(x), static_cast<float>(y));
        mousePositionLastFrame_ = mousePosition_;
        mouseDelta_ = Vec2(0.0f);
    } 
    else if (cursorVisible_) 
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } 
    else 
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

bool Input::IsAnyKeyPressed() const 
{
    for (uint16 i{0}; i < MAX_KEYS; ++i) 
    {
        if (keys_[i]) 
            return true;
    }
    return false;
}

bool Input::IsAnyMouseButtonPressed() const 
{
    for (uint8 i{0}; i < MAX_MOUSE_BUTTONS; ++i)
    {
        if (mouseButtons_[i]) 
            return true;
    }
    return false;
}

bool Input::IsShiftPressed() const {
    return IsKeyPressed(KeyCode::LeftShift) || IsKeyPressed(KeyCode::RightShift);
}

bool Input::IsControlPressed() const {
    return IsKeyPressed(KeyCode::LeftControl) || IsKeyPressed(KeyCode::RightControl);
}

bool Input::IsAltPressed() const {
    return IsKeyPressed(KeyCode::LeftAlt) || IsKeyPressed(KeyCode::RightAlt);
}

bool Input::IsSuperPressed() const {
    return IsKeyPressed(KeyCode::LeftSuper) || IsKeyPressed(KeyCode::RightSuper);
}

} // namespace TLETC

