#pragma once

#include "TLETC/Rendering/Camera.h"
#include "TLETC/Scene/Behaviour.h"

namespace TLETC {

/**
 * OrbitCamera - Camera that orbits around a target point
 * 
 * Inherits from both Camera and Behaviour.
 * Can be used as a Camera and receives input events as a Behaviour.
 * 
 * Controls:
 * - Left Mouse Drag: Tumble (rotate around target)
 * - Right Mouse Drag: Pan (move target)
 * - Mouse Wheel: Zoom in/out
 * 
 * Usage:
 *   auto* camEntity = app.CreateEntity("Camera");
 *   auto* orbitCam = camEntity->AddBehaviour<OrbitCamera>();
 *   orbitCam->SetTarget(Vec3(0, 0, 0));
 *   orbitCam->SetDistance(10.0f);
 *   app.SetCamera(orbitCam);  // OrbitCamera IS a Camera!
 */
class OrbitCamera : public Camera, public Behaviour 
{
public:
    OrbitCamera();

    // Target
    void SetTarget(const Vec3& target);
    Vec3 GetTarget() const { return targetPoint_; }
    
    // Distance from target
    void SetDistance(float distance);
    float GetDistance() const { return distance_; }
    
    // Zoom limits
    void SetMinDistance(float minDist) { minDistance_ = minDist; }
    void SetMaxDistance(float maxDist) { maxDistance_ = maxDist; }
    float GetMinDistance() const { return minDistance_; }
    float GetMaxDistance() const { return maxDistance_; }
    
    // Rotation (in degrees)
    void SetYaw(float yaw);
    void SetPitch(float pitch);
    float GetYaw() const   { return yaw_; }
    float GetPitch() const { return pitch_; }
    
    // Sensitivity
    void SetRotationSpeed(float speed) { rotationSpeed_ = speed; }
    void SetPanSpeed(float speed)      { panSpeed_ = speed; }
    void SetZoomSpeed(float speed)     { zoomSpeed_ = speed; }
    float GetRotationSpeed() const     { return rotationSpeed_; }
    float GetPanSpeed() const          { return panSpeed_; }
    float GetZoomSpeed() const         { return zoomSpeed_; }
    
    const char* GetName() const override { return "OrbitCamera"; }

    protected:
    void OnInit() override;
    void OnMouseButtonPressed(MouseButton button) override;
    void OnMouseButtonReleased(MouseButton button) override;
    void OnMouseMoved(const Vec2& position, const Vec2& delta) override;
    void OnMouseScrolled(const Vec2& offset) override;
    
private:
    void UpdateCameraTransform();
    
    // Target and distance
    Vec3  targetPoint_;
    float distance_;
    float minDistance_;
    float maxDistance_;
    
    // Rotation (spherical coordinates)
    float yaw_;    // Horizontal rotation (around Y axis)
    float pitch_;  // Vertical rotation
    
    // Input state
    bool tumbling_;  // Left mouse dragging
    bool panning_;   // Ctrl + Left mouse dragging
    
    // Sensitivity
    float rotationSpeed_;
    float panSpeed_;
    float zoomSpeed_;
};

} // namespace TLETC 