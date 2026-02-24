#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Scene.h"

namespace TLETC::ECS
{

const mat4& Transform::GetWorldMatrix(Scene& scene)
{
    if(worldDirty_)
    {
        RebuildWorldMatrix(scene);
        worldDirty_ = false;
    }
    return worldMatrix_;
}

void RebuildWorldMatrix(Scene& scene)
{
    
}

vec3 Transform::GetWorldPosition() const
{
    return vec3(worldMatrix_[3]); 
} 

quat Transform::GetWorldRotation() const
{
    // TODO: maybe add gram-schmidt to remove shears
    vec3 scale = GetWorldScale();
    
    mat3 rotMatrix;
    rotMatrix[0] = vec3(worldMatrix_[0]) / scale_.x;
    rotMatrix[1] = vec3(worldMatrix_[1]) / scale_.y;
    rotMatrix[2] = vec3(worldMatrix_[2]) / scale_.z;
    
    return quat_cast(rotMatrix);
}

vec3 Transform::GetWorldScale() const 
{
    // Extract scale from world matrix column lengths
    vec3 scale;
    scale.x = length(vec3(worldMatrix_[0]));
    scale.y = length(vec3(worldMatrix_[1]));
    scale.z = length(vec3(worldMatrix_[2]));
    return scale;  
}

void Transform::SetWorldPosition(const vec3& pos, Scene* scene) 
{
    if(parent_.IsNull() || !scene)
    {
        // No parent - local IS world
        position_ = pos; 
    }
    else
    {  
        // Convert world to local
        Transform* parentT = scene->GetComponent<Transform>(parent_);
        if(parentT)
        {
            // local = inverse(parent.world)*world
            mat4 parentInverse = inverse(parentT->worldMatrix_);
            vec4 localPos4     = parentInverse*vec4(pos, 1.0f);
            position_          = vec3(localPos4);
        }
        else
        {
            position_ = pos;
        }
    }
    dirty_ = true;
}

void Transform::SetWorldRotation(const quat& rot, Scene* scene) 
{
    if(parent_.IsNull() || !scene)
    {
        // No parent - local IS world
        rotation_ = rot; 
    }
    else
    {  
        // Convert world to local
        Transform* parentT = scene->GetComponent<Transform>(parent_);
        if(parentT)
        {
            // local = inverse(parent.world)*world
            quat parentRot = parentT->GetWorldRotation();
            rotation_ = inverse(parentRot) * rot;
        }
        else
        {
            rotation_ = rot;
        }
    }
    dirty_ = true;
}

void Transform::SetWorldScale(const vec3& scale, Scene* scene) 
{
    if (parent_.IsNull() || !scene)
    {
        scale_ = scale;
    }
    else
    {
        Transform* parentT = scene->GetComponent<Transform>(parent_);
        if (parentT)
        {
            // local_scale = world_scale / parent_scale
            vec3 parentScale = parentT->GetWorldScale();
            scale_ = scale / parentScale;
        }
        else
        {
            scale_ = scale;
        }
    }
    
    dirty_ = true;
}

vec3 Transform::Forward() const
{
    return normalize(vec3(worldMatrix_[2]));  // Z axis
}

vec3 Transform::Right() const
{
    return normalize(vec3(worldMatrix_[0]));  // X axis
}

vec3 Transform::Up() const
{
    return normalize(vec3(worldMatrix_[1]));  // Y axis
}

void Transform::LookAt(const vec3& target, const vec3& worldUp, Scene& scene)
{
    vec3 worldPos = GetWorldPosition();
    vec3 forward = normalize(target - worldPos);
    
    if (length(forward) < 0.001f)
        return;
    
    vec3 right = normalize(cross(worldUp, forward));
    vec3 up = cross(forward, right);
    
    mat3 rotMatrix;
    rotMatrix[0] = right;
    rotMatrix[1] = up;
    rotMatrix[2] = forward;
    
    quat worldRot = quat_cast(rotMatrix);
    
    // Convert to local space
    SetWorldRotation(worldRot, &scene);
}

} // namespace TLETC::ECS