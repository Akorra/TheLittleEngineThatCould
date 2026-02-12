#include "TLETC/Platform/Input.h"

namespace TLETC 
{
void Input::BeginFrame()
{
    mouseDelta_ = vec2(0.0f);
    scrollDelta_ = vec2(0.0f);

    for (auto& key : keys_)
        key.previous = key.current;

    for (auto& button : mouseButtons_)
        button.previous = button.current;
}

void Input::OnKeyEvent(KeyCode key, bool pressed)
{
    uint16 index = static_cast<uint16>(key);
    if (index >= MAX_KEYS)
        return;
    keys_[index].current = pressed;
}

void Input::OnMouseButton(MouseButton button, bool pressed)
{
    uint8 index = static_cast<uint8>(button);
    if (index >= MAX_MOUSE_BUTTONS)
        return;
    mouseButtons_[index].current = pressed;
}

void Input::OnMouseMove(float x, float y)
{
    mousePosition_ = vec2(x, y);
    mouseDelta_ = mousePosition_ - lastMousePosition_;
    lastMousePosition_ = mousePosition_;
}

void Input::OnMouseScroll(float xOffset, float yOffset)
{
    scrollDelta_ = vec2(xOffset, yOffset);
}

bool Input::IsKeyDown(KeyCode key) const
{
    return keys_[static_cast<uint16>(key)].current;
}

bool Input::IsKeyPressed(KeyCode key) const
{
    auto& k = keys_[static_cast<uint16>(key)];
    return k.current && !k.previous;
}

bool Input::IsKeyReleased(KeyCode key) const
{
    auto& k = keys_[static_cast<uint16>(key)];
    return !k.current && k.previous;
}

bool Input::IsMouseButtonDown(MouseButton button) const
{
    return mouseButtons_[static_cast<uint8>(button)].current;
}

bool Input::IsMouseButtonPressed(MouseButton button) const
{
    auto& b = mouseButtons_[static_cast<uint8>(button)];
    return b.current && !b.previous;
}

bool Input::IsMouseButtonReleased(MouseButton button) const
{
    auto& b = mouseButtons_[static_cast<uint8>(button)];
    return !b.current && b.previous;
}

} // namespace TLETC

