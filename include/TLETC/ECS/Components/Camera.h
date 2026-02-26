#pragma once

#include "TLETC/Core/Math.h"

namespace TLETC::ECS
{

enum class ProjectionType
{
    Perspective,
    Orthographic
};

struct Camera
{
    ProjectionType type = ProjectionType::Perspective;

    // Perspective
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    // Orthographic
    float orthoSize = 10.0f;

    // Cached matrices
    mat4 projection;
    mat4 view;

    bool isPrimary = false;  // Only one camera should be primary

    mat4 GetProjectionMatrix(float aspect) const
    {
        if (type == ProjectionType::Perspective)
        {
            return perspective(radians(fov), aspect, nearPlane, farPlane);
        }
        else
        {
            float halfSize = orthoSize * 0.5f;
            return ortho(-halfSize * aspect, halfSize * aspect,
                        -halfSize, halfSize,
                        nearPlane, farPlane);
        }
    }
};

} // namespace TLETC::ECS