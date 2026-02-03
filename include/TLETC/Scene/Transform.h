#pragma once

#include "TLETC/Core/Math.h"

namespace TLETC {

class Transform {
public:
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    
    Transform* parent;
    
    Transform()
        : position(0.0f, 0.0f, 0.0f)
        , rotation(1.0f, 0.0f, 0.0f, 0.0f)  // Identity quaternion
        , scale(1.0f, 1.0f, 1.0f)
        , parent(nullptr)
    {}
    
    // Get the local transformation matrix
    Mat4 GetModelMatrix() const {
        Mat4 S = glm::scale(Mat4(1.0f), scale);
        Mat4 R = glm::mat4_cast(rotation);
        Mat4 T = glm::translate(Mat4(1.0f), position);
        return T * R * S;
    }
    
    // Get the world transformation matrix (including parent transforms)
    Mat4 GetWorldMatrix() const {
        Mat4 localMatrix = GetModelMatrix();
        if (parent) {
            return parent->GetWorldMatrix() * localMatrix;
        }
        return localMatrix;
    }
    
    // Get world position
    Vec3 GetWorldPosition() const {
        if (parent) {
            Mat4 worldMatrix = GetWorldMatrix();
            return Vec3(worldMatrix[3]);
        }
        return position;
    }
    
    // Direction vectors
    Vec3 GetForward() const {
        return rotation * Vec3(0.0f, 0.0f, -1.0f);
    }
    
    Vec3 GetRight() const {
        return rotation * Vec3(1.0f, 0.0f, 0.0f);
    }
    
    Vec3 GetUp() const {
        return rotation * Vec3(0.0f, 1.0f, 0.0f);
    }
    
    // Transform helpers
    void Rotate(const Vec3& axis, float angleDegrees) {
        rotation = QuatFromAxisAngle(axis, Radians(angleDegrees)) * rotation;
        rotation = normalize(rotation);
    }

    void Translate(const Vec3& delta) {
        position += delta;
    }

    void SetScale(const Vec3& s) {
        scale = s;
    }
    
    void LookAt(const Vec3& target, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f)) {
        Vec3 direction = normalize(target - position);
        Mat4 lookMatrix = glm::lookAt(position, target, up);
        rotation = glm::quat_cast(glm::inverse(lookMatrix));
    }
    
    // Hierarchy
    void SetParent(Transform* newParent) {
        parent = newParent;
    }
};

} // namespace TLETC