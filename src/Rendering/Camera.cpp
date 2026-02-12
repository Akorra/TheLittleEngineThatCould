#include "TLETC/Rendering/Camera.h"

namespace TLETC 
{

Mat4 Camera::GetViewMatrix() const 
{
    return lookAt(position_, target_, up_);
}

Mat4 Camera::GetProjectionMatrix(float aspectRatio) const 
{
    if (projectionType_ == ProjectionType::Perspective) 
    {
        return perspective(radians(fov_), aspectRatio, nearPlane_, farPlane_);
    } 
    else 
    {
        // Orthographic
        float halfWidth = orthoSize_ * aspectRatio;
        float halfHeight = orthoSize_;
        return ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane_, farPlane_);
    }
}

void Camera::SetPerspective(float fovDegrees, float near, float far) 
{
    projectionType_ = ProjectionType::Perspective;

    fov_ = fovDegrees;
    nearPlane_ = near;
    farPlane_ = far;
}

void Camera::SetOrthographic(float size, float near, float far) 
{
    projectionType_ = ProjectionType::Orthographic;
    orthoSize_ = size;
    nearPlane_ = near;
    farPlane_  = far;
}

} // namespace TLETC