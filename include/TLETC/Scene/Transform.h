#pragma once

#include "TLETC/Core/Math.h"

namespace TLETC {

class Transform {
public:
    Vec3 position_;
    Quat rotation_;
    Vec3 scale_;
    
    Transform* parent_;
    
    Transform()
        : position_(0.0f, 0.0f, 0.0f)
        , rotation_(1.0f, 0.0f, 0.0f, 0.0f)  // Identity quaternion
        , scale_(1.0f, 1.0f, 1.0f)
        , parent_(nullptr)
    {}
    
    // Get the local transformation matrix
    Mat4 GetModelMatrix() const {
        Mat4 S = scale(Mat4(1.0f), scale_);
        Mat4 R = mat4_cast(rotation_);
        Mat4 T = translate(Mat4(1.0f), position_);
        return T * R * S;
    }
    
    // Get the world transformation matrix (including parent transforms)
    Mat4 GetWorldMatrix() const {
        Mat4 localMatrix = GetModelMatrix();
        if (parent_) 
            return parent_->GetWorldMatrix() * localMatrix;
        
        return localMatrix;
    }
    
    // Get world position
    Vec3 GetWorldPosition() const {
        if (parent_) {
            Mat4 worldMatrix = GetWorldMatrix();
            return Vec3(worldMatrix[3]);
        }
        return position_;
    }
    
    // Direction vectors
    Vec3 GetForward() const {
        return rotation_ * Vec3(0.0f, 0.0f, -1.0f);
    }
    
    Vec3 GetRight() const {
        return rotation_ * Vec3(1.0f, 0.0f, 0.0f);
    }
    
    Vec3 GetUp() const {
        return rotation_ * Vec3(0.0f, 1.0f, 0.0f);
    }
    
    // Transform helpers
    void Rotate(const Vec3& axis, float angleDegrees) {
        rotation_ = angleAxis(radians(angleDegrees), axis) * rotation_;
        rotation_ = normalize(rotation_);
    }

    void Translate(const Vec3& delta) {
        position_ += delta;
    }

    void SetScale(const Vec3& s) {
        scale_ = s;
    }
    
    void LookAt(const Vec3& target, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f)) {
        Vec3 direction = normalize(target - position_);
        Mat4 lookMatrix = lookAt(position_, target, up);
        rotation_ = quat_cast(inverse(lookMatrix));
    }
    
    // Hierarchy
    void SetParent(Transform* newParent) {
        parent_ = newParent;
    }
};

} // namespace TLETC