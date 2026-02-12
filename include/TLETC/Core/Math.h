#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TLETC
{

// Type aliases for GLM types
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

using Quat = glm::quat;

// Common math functions
using glm::radians;
using glm::degrees;
using glm::normalize;
using glm::length;
using glm::dot;
using glm::cross;
using glm::mix;
using glm::clamp;

// Matrix operations
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::lookAt;
using glm::perspective;
using glm::ortho;

// Quaternion operations
using glm::angleAxis;
using glm::slerp;
using glm::sin;
using glm::cos;

// Utility functions
inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float TWO_PI = 2.0f * PI;
inline constexpr float HALF_PI = 0.5f * PI;

// Bounding box structure
struct BoundingBox {
    Vec3 min;
    Vec3 max;
    
    BoundingBox() 
        : min(Vec3(0.0f)), max(Vec3(0.0f)) {}
    
    BoundingBox(const Vec3& min, const Vec3& max)
        : min(min), max(max) {}
    
    Vec3 GetCenter() const {
        return (min + max) * 0.5f;
    }
    
    Vec3 GetSize() const {
        return max - min;
    }
    
    bool Contains(const Vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
};

} // namespace TLETC