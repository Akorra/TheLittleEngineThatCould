#include "TLETC/Rendering/Camera.h"

namespace TLETC 
{

Mat4 Camera::GetViewMatrix() const 
{
    return lookAt(position, target, up);
}

Mat4 Camera::GetProjectionMatrix(float aspectRatio) const 
{
    return perspective(radians(fov), aspectRatio, nearPlane, farPlane);
}

} // namespace TLETC