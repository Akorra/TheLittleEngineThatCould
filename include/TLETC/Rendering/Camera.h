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
    enum class ClearFlags { All, ColorOnly, DepthOnly, Nothing };
    enum class ProjectionType { Perspective, Orthographic };

    Camera() = default;
    virtual ~Camera() = default;

    void        SetPosition(Vec3 pos) { position_ = pos; }
    const Vec3& GetPosition() const    { return position_; }

    void        SetCameraUp(Vec3 up) { up_ = up; }
    const Vec3& GetCameraUp() const { return up_; }

    void        SetTarget(Vec3 target) { target_ = target; }
    const Vec3& GetTarget()   const { return target_; }
    
    float GetFOV()  const { return fov_; }
    float GetNear() const { return nearPlane_; }
    float GetFar()  const { return farPlane_; }

    // Get matrices
    virtual Mat4 GetViewMatrix() const;
    virtual Mat4 GetProjectionMatrix(float aspectRatio) const;

    // Helpers
    void SetPerspective() { projectionType_ = ProjectionType::Perspective;  }; //< use stored values
    void SetOrtographic() { projectionType_ = ProjectionType::Orthographic; }; //< use stored values
    void SetPerspective(float fovDegrees, float near, float far);
    void SetOrthographic(float size, float near, float far);

    void SetCullingMask(uint32 mask) { cullingMask_ = mask; }
    uint32_t GetCullingMask() const  { return cullingMask_; }

    void       SetClearFlags(ClearFlags flags) { clearFlags_ = flags; }
    ClearFlags GetClearFlags() const { return clearFlags_; }
    
    void        SetClearColor(Vec4 color) { clearColor_ = color; }
    const Vec4& GetClearColor() const     { return clearColor_; }

    // Multicamera support
    void SetRenderOrder(int order) { renderOrder_ = order; }
    int  GetRenderOrder() const    { return renderOrder_; }

private:
    // Projection type
    ProjectionType projectionType_ = ProjectionType::Perspective;

    // Transform
    Vec3 position_ = Vec3(0, 0, 5);
    Vec3 target_   = Vec3(0, 0, 0);
    Vec3 up_       = Vec3(0, 1, 0);
    Vec4 clear_    = Vec4(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Projection parameters
    float fov_ = 45.0f;        // Field of view in degrees
    float nearPlane_ = 0.1f;
    float farPlane_ = 100.0f;

    uint32_t   cullingMask_ = 0xFFFFFFFF; // everything
    ClearFlags clearFlags_  = ClearFlags::All;
    Vec4       clearColor_  = Vec4(0,0,0,1);
    
    // Orthographic parameters
    float orthoSize_ = 10.0f;  // Half-height of view volume
    int   renderOrder_ = 0; // lower renders first
};

} // namespace TLETC
