#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"

struct GLFWwindow;

namespace TLETC 
{
// Key codes (matching GLFW for now, abstracted)
enum class KeyCode 
{
    // Printable keys
    Space      = 32,
	Apostrophe = 39, /* ' */
	Comma      = 44, /* , */
	Minus      = 45, /* - */
	Period     = 46, /* . */
	Slash      = 47, /* / */

    D0 = 48, D1 = 49, D2 = 50, D3 = 51, D4 = 52,
    D5 = 53, D6 = 54, D7 = 55, D8 = 56, D9 = 57,

    Semicolon = 59, /* ; */
	Equal     = 61, /* = */

    A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71,
    H = 72, I = 73, J = 74, K = 75, L = 76, M = 77, N = 78,
    O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85,
    V = 86, W = 87, X = 88, Y = 89, Z = 90,

    LeftBracket  = 91, /* [ */
    Backslash    = 92, /* \ */
    RightBracket = 93, /* ] */
    GraveAccent  = 96, /* ` */

    World1 = 161, /* non-US #1 */
	World2 = 162, /* non-US #2 */

    // Function keys
    Escape      = 256,
    Enter       = 257,
    Tab         = 258,
    Backspace   = 259,
    Insert      = 260,
    Delete      = 261,
    Right       = 262,
    Left        = 263,
    Down        = 264,
    Up          = 265,
    PageUp      = 266,
    PageDown    = 267,
    Home        = 268,
    End         = 269,
    CapsLock    = 280,
    ScrollLock  = 281,
    NumLock     = 282,
    PrintScreen = 283,
    Pause       = 284,

    F1  = 290, F2  = 291, F3  = 292, F4  = 293, F5  = 294,
    F6  = 295, F7  = 296, F8  = 297, F9  = 298, F10 = 299,
    F11 = 300, F12 = 301, F13 = 302, F14 = 303, F15 = 304,
    F16 = 305, F17 = 306, F18 = 307, F19 = 308, F20 = 309, 
    F21 = 310, F22 = 311, F23 = 312, F25 = 313, F25 = 314,

    // Keypad
    KP0 = 320, KP1 = 321, KP2 = 322, KP3 = 323, KP4 = 324,
    KP5 = 325, KP6 = 326, KP7 = 327, KP8 = 328, KP9 = 329,
    KPDecimal  = 330,
    KPDivide   = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd      = 334,
    KPEnter    = 335,
    KPEqual    = 336,
    
    // Modifiers
    LeftShift    = 340,
    LeftControl  = 341,
    LeftAlt      = 342,
    LeftSuper    = 343,
    RightShift   = 344,
    RightControl = 345,
    RightAlt     = 346,
    RightSuper   = 347,
    Menu         = 348
};

// Mouse button codes
enum class MouseButton {
    Left    = 0,
    Right   = 1,
    Middle  = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

/**
 * Input - Manages keyboard and mouse input
 * 
 * Provides a simple, polling-based input system.
 * Call Update() once per frame to refresh input state.
 */
class Input 
{
public:
    Input();
    ~Input();
    
    // Initialization
    void Initialize(GLFWwindow* window);
    void Shutdown();
    
    // Update - call once per frame
    void Update();
    
    // Keyboard input
    bool IsKeyPressed(const KeyCode& key) const;      // Is key currently held down?
    bool IsKeyJustPressed(const KeyCode& key) const;  // Was key pressed this frame?
    bool IsKeyJustReleased(const KeyCode& key) const; // Was key released this frame?
    
    // Mouse buttons
    bool IsMouseButtonPressed(const MouseButton& button) const;
    bool IsMouseButtonJustPressed(const MouseButton& button) const;
    bool IsMouseButtonJustReleased(const MouseButton& button) const;
    
    // Mouse position
    Vec2  GetMousePosition() const { return mousePosition_; }
    Vec2  GetMouseDelta()    const { return mouseDelta_; }
    float GetMouseX()        const { return mousePosition_.x; }
    float GetMouseY()        const { return mousePosition_.y; }
    
    // Mouse scroll
    Vec2  GetMouseScroll()  const { return mouseScroll_; }
    float GetMouseScrollX() const { return mouseScroll_.x; }
    float GetMouseScrollY() const { return mouseScroll_.y; }
    
    // Mouse cursor control
    void SetCursorVisible(bool visible);
    void SetCursorLocked(bool locked);
    bool IsCursorVisible() const { return cursorVisible_; }
    bool IsCursorLocked()  const { return cursorLocked_; }
    
    // Helper methods
    bool IsAnyKeyPressed() const;
    bool IsAnyMouseButtonPressed() const;
    
    // Modifier keys
    bool IsShiftPressed() const;
    bool IsControlPressed() const;
    bool IsAltPressed() const;
    bool IsSuperPressed() const;
    
private:
    GLFWwindow* window_;
    
    // Key state
    static const uint16 MAX_KEYS = 512;
    bool keys_[MAX_KEYS];
    bool keysLastFrame_[MAX_KEYS];
    
    // Mouse button state
    static const uint8 MAX_MOUSE_BUTTONS = 8;
    bool mouseButtons_[MAX_MOUSE_BUTTONS];
    bool mouseButtonsLastFrame_[MAX_MOUSE_BUTTONS];
    
    // Mouse position
    Vec2 mousePosition_;
    Vec2 mousePositionLastFrame_;
    Vec2 mouseDelta_;
    
    // Mouse scroll
    Vec2 mouseScroll_;
    
    // Cursor state
    bool cursorVisible_;
    bool cursorLocked_;
    
    // Callback friends
    friend void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

} // namespace TLETC