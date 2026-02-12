#pragma once

// =======================================================
// Math Type Aliases (GLM)
// =======================================================

// Include GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TLETC {

// -------------------------------------------------------
// Vectors
// -------------------------------------------------------
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::bvec2;
using glm::bvec3;
using glm::bvec4;

// -------------------------------------------------------
// Matrices
// -------------------------------------------------------
using glm::mat2;
using glm::mat3;
using glm::mat4;

// -------------------------------------------------------
// Quaternions
// -------------------------------------------------------
using glm::quat;

// -----------------------------------------------------------------------------
// Common math functions.
// -----------------------------------------------------------------------------
using glm::radians;
using glm::degrees;
using glm::normalize;
using glm::length;
using glm::dot;
using glm::cross;
using glm::mix;
using glm::clamp;

// -----------------------------------------------------------------------------
// Common math functions.
// -----------------------------------------------------------------------------
using glm::radians;
using glm::degrees;
using glm::normalize;
using glm::length;
using glm::dot;
using glm::cross;
using glm::mix;
using glm::clamp;

// -----------------------------------------------------------------------------
// Matrix operations.
// -----------------------------------------------------------------------------
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::lookAt;
using glm::perspective;
using glm::ortho;

// -----------------------------------------------------------------------------
// Quaternion operations.
// -----------------------------------------------------------------------------
using glm::angleAxis;
using glm::slerp;
using glm::sin;
using glm::cos;

// -----------------------------------------------------------------------------
// Math constants.
// -----------------------------------------------------------------------------
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = 2.0f * kPi;
inline constexpr float kHalfPi = 0.5f * kPi;

// -----------------------------------------------------------------------------
// Axis-aligned bounding box.
// -----------------------------------------------------------------------------
struct BoundingBox 
{
  vec3 min;
  vec3 max;

  BoundingBox() : min(0.0f), max(0.0f) {}

  BoundingBox(const vec3& min, const vec3& max) : min(min), max(max) {}

  vec3 GetCenter() const {
    return (min + max) * 0.5f;
  }

  vec3 GetSize() const {
    return max - min;
  }

  bool Contains(const vec3& point) const {
    return (point.x >= min.x && point.x <= max.x) &&
           (point.y >= min.y && point.y <= max.y) &&
           (point.z >= min.z && point.z <= max.z);
  }
};

} // namespace TLETC