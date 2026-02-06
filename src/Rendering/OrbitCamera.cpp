#include "TLETC/Rendering/OrbitCamera.h"

namespace TLETC {

OrbitCamera::OrbitCamera()
    : targetPoint_(Vec3(0, 0, 0))
    , distance_(10.0f)
    , minDistance_(1.0f)
    , maxDistance_(100.0f)
    , yaw_(0.0f)
    , pitch_(30.0f)
    , tumbling_(false)
    , panning_(false)
    , rotationSpeed_(0.5f)
    , panSpeed_(0.01f)
    , zoomSpeed_(1.0f)
{
    // Register for input events
    SetActiveEvents(EventFlag::MouseButtonEvents | EventFlag::MouseMoveEvents | EventFlag::MouseScrollEvents | EventFlag::KeyEvents);
}

void OrbitCamera::OnInit() 
{
    UpdateCameraTransform();
}

void OrbitCamera::SetTarget(const Vec3& target) 
{
    targetPoint_ = target;
    UpdateCameraTransform();
}

void OrbitCamera::SetDistance(float distance) 
{
    distance_ = clamp(distance, minDistance_, maxDistance_);
    UpdateCameraTransform();
}

void OrbitCamera::SetYaw(float yaw) 
{
    yaw_ = yaw;
    UpdateCameraTransform();
}

void OrbitCamera::SetPitch(float pitch) 
{
    pitch_ = clamp(pitch, -89.0f, 89.0f);
    UpdateCameraTransform();
}

void OrbitCamera::OnMouseButtonPressed(MouseButton button) 
{
    if (button == MouseButton::Left)
        tumbling_ = true;
    else if (button == MouseButton::Right)
        panning_ = true;
}

void OrbitCamera::OnMouseButtonReleased(MouseButton button) 
{
    if (button == MouseButton::Left)
        tumbling_ = false;
    else if (button == MouseButton::Right)
        panning_ = false;
}

void OrbitCamera::OnMouseMoved(const Vec2& position, const Vec2& delta) 
{
    (void) position;
    
    if (tumbling_) 
    {
        // Tumble: Rotate around target
        yaw_ -= delta.x * rotationSpeed_;
        pitch_ -= delta.y * rotationSpeed_;
        
        // Clamp pitch to avoid gimbal lock
        pitch_ = clamp(pitch_, -89.0f, 89.0f);
        
        UpdateCameraTransform();
    } 
    else if (panning_) 
    {
        // Pan: Move target in camera's local XY plane
        Vec3 forward = normalize(targetPoint_ - position_);
        Vec3 right = normalize(cross(forward, up_));
        Vec3 upVec = cross(right, forward);
        
        targetPoint_ -= right * delta.x * panSpeed_ * distance_;
        targetPoint_ += upVec * delta.y * panSpeed_ * distance_;
        
        UpdateCameraTransform();
    }
}

void OrbitCamera::OnMouseScrolled(const Vec2& offset) {
    // Zoom: Change distance from target
    distance_ -= offset.y * zoomSpeed_;
    distance_ = glm::clamp(distance_, minDistance_, maxDistance_);
    
    UpdateCameraTransform();
}

void OrbitCamera::UpdateCameraTransform() {
    // Convert spherical coordinates to Cartesian
    float yawRad = radians(yaw_);
    float pitchRad = radians(pitch_);
    
    Vec3 offset;
    offset.x = distance_ * cos(pitchRad) * sin(yawRad);
    offset.y = distance_ * sin(pitchRad);
    offset.z = distance_ * cos(pitchRad) * cos(yawRad);
    
    // Update Camera base class members
    position_ = targetPoint_ + offset;
    target_   = targetPoint_;
}

} // namespace TLETC