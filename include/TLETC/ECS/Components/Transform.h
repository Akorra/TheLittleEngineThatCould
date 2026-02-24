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
    bool worldDirty_ = true;

    struct TransformConstraints {
        bool lockX = false;
        bool lockY = false;
        bool lockZ = false;
    } constrains_;

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

    // World Space Helpers - column major (glm)
    vec3 GetWorldPosition() const;
    quat GetWorldRotation() const;
    vec3 GetWorldScale()    const;

    void SetWorldPosition(const vec3& pos, Scene* scene=nullptr);
    void SetWorldRotation(const quat& rot, Scene* scene=nullptr);
    void SetWorldScale(const vec3& scale,  Scene* scene=nullptr);

    const mat4& GetWorldMatrix(Scene& scene);
    void  RebuildWorldMatrix(Scene& scene);

    // Directions
    vec3 Forward() const;
    vec3 Right() const;
    vec3 Up() const;

    // Target
    void LookAt(const vec3& target, const vec3& worldUp, Scene& scene);
};

} // namespace TLETC::ECS
