#pragma once

#include "TLETC/Core/Math.h"
#include "TLETC/ECS/Entity.h"
#include <vector>

namespace TLETC::ECS
{
/**
 * Transform Component
 * 
 * Local Space: relative to parent (or worlf if no parent)
 * World space: absolute position in the scene
 * 
 * World matrix is rebuilt during TransformSystem::FixedUpdate [when dirty flag is set].
 */
struct Transform 
{
    // Local Transform
    vec3 position_{0.0f};
    quat rotation_{1.0f, 0.0f, 0.0f, 0.0f}; //< Identity
    vec3 scale_{1.0f};

    // World Transform
    mat4 worldMatrix_{1.0f};     //< current frame
    mat4 prevWorldMatrix_{1.0f}; //< previous frame

    // Hierarchy
    Entity parent_ = Entity::Null();
    std::vector<Entity> children_;

    // State
    bool dirty_ = true; // Needs matrix rebuild

    // Helpers
    bool IsRoot() const { return !parent_; } 
    bool HasChildren() const { return !children_.empty(); }

    // Get Local TRS matrix
    mat4 GetLocalMatrix() const 
    {
        mat4 T = translate(mat4(1.0f), position_);
        mat4 R = mat4_cast(rotation_);
        mat4 S = scale(mat4(1.0), scale_);
        return T*R*S;
    }

    // Get world position (from matrix)
    vec3 GetWorldPosition() const { return vec3(worldMatrix_[3]); }

    // Set world position (breaks hierarchy - use carefully!)
    void SetWorldPosition(const vec3& pos)
    {
        worldMatrix_[3] = vec4(pos, 1.0f);
        dirty_ = true;
    }
};

} // namespace TLETC::ECS
