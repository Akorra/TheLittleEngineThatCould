#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"

namespace TLETC {

/**
 * Camera - Provides view and projection matrices for rendering
 * 
 * Usage:
 *   Camera camera;
 *   camera.position = Vec3(0, 5, 10);
 *   camera.target = Vec3(0, 0, 0);
 *   app.SetCamera(&camera);
 */
class Camera
{
public:
    // Transform
    Vec3 position = Vec3(0, 0, 5);
    Vec3 target = Vec3(0, 0, 0);
    Vec3 up = Vec3(0, 1, 0);
    
    // Projection parameters
    float fov = 45.0f;        // Field of view in degrees
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    // Get matrices
    Mat4 GetViewMatrix() const;
    Mat4 GetProjectionMatrix(float aspectRatio) const;
};

} // namespace TLETC
