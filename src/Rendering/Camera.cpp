#include "TLETC/Rendering/Camera.h"

namespace TLETC 
{

Mat4 Camera::GetViewMatrix() const 
{
    return lookAt(position, target, up);
}

Mat4 Camera::GetProjectionMatrix(float aspectRatio) const 
{
    if (projectionType == ProjectionType::Perspective) 
    {
        return perspective(radians(fov), aspectRatio, nearPlane, farPlane);
    } 
    else 
    {
        // Orthographic
        float halfWidth = orthoSize * aspectRatio;
        float halfHeight = orthoSize;
        return ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }
}

void Camera::SetPerspective(float fovDegrees, float near, float far) 
{
    projectionType = ProjectionType::Perspective;

    fov = fovDegrees;
    nearPlane = near;
    farPlane = far;
}

void Camera::SetOrthographic(float size, float near, float far) 
{
    projectionType = ProjectionType::Orthographic;
    orthoSize = size;
    nearPlane = near;
    farPlane = far;
}

} // namespace TLETC