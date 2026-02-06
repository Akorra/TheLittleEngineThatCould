#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"

namespace TLETC {

/**
 * Camera - Base camera class
 * 
 * Provides view and projection matrices for rendering.
 * Can be perspective or orthographic.
 * 
 * Usage:
 *   Camera camera;
 *   camera.position = Vec3(0, 5, 10);
 *   camera.target = Vec3(0, 0, 0);
 *   camera.SetPerspective(45.0f, 0.1f, 100.0f);
 *   app.SetCamera(&camera);
 */
class Camera
{
public:
    enum class ProjectionType { Perspective, Orthographic };

    Camera();
    virtual ~Camera() = default;

    // Projection type
    ProjectionType projectionType_ = ProjectionType::Perspective;

    // Transform
    Vec3 position_ = Vec3(0, 0, 5);
    Vec3 target_ = Vec3(0, 0, 0);
    Vec3 up_ = Vec3(0, 1, 0);
    
    // Projection parameters
    float fov_ = 45.0f;        // Field of view in degrees
    float nearPlane_ = 0.1f;
    float farPlane_ = 100.0f;
    
    // Orthographic parameters
    float orthoSize_ = 10.0f;  // Half-height of view volume
    
    // Get matrices
    virtual Mat4 GetViewMatrix() const;
    virtual Mat4 GetProjectionMatrix(float aspectRatio) const;
    
    // Helpers
    void SetPerspective() { projectionType_ = ProjectionType::Perspective;  }; //< use stored values
    void SetOrtographic() { projectionType_ = ProjectionType::Orthographic; }; //< use stored values
    void SetPerspective(float fovDegrees, float near, float far);
    void SetOrthographic(float size, float near, float far);
};

} // namespace TLETC
