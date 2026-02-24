#include "TLETC/ECS/Systems/TransformSystem.h"
#include "TLETC/ECS/Components/Transform.h"
#include "TLETC/ECS/Scene.h"

namespace TLETC::ECS
{

void TransformSystem::FixedUpdate(Scene& scene, float dt)
{
    // Process all root transforms
    auto roots = scene.GetRootEntities();
    for(Entity root : roots)
        UpdateRecursive(scene, root, false);
}

void TransformSystem::UpdateRecursive(Scene& scene, Entity entity, bool parentDirty)
{
    Transform* t = scene.GetComponent<Transform>(entity);
    if (!t) return;

    // Update this transform if it's dirty OR parent was dirty
    if(t->dirty_ || parentDirty)
    {
        // Save previous world matrix for interpolation
        t->prevWorldMatrix_ = t->worldMatrix_;

        // Build local TRS matrix
        mat4 localMatrix = t->GetLocalMatrix();

        // Combine with parent's world matrix (if has parent)
        if ( !(t->parent_.IsNull()) && scene.IsValid(t->parent_) )
        {
            Transform* parentT = scene.GetComponent<Transform>(t->parent_);
            t->worldMatrix_ = parentT ? parentT->worldMatrix_ * localMatrix : localMatrix;
        }
        else
        {
            t->worldMatrix_ = localMatrix; //< Root transform - local IS world
        }

        // Clear dirty flag
        t->dirty_ = false;

        // Mark that children need update
        parentDirty = true;
    }

    // Recursively update children [copy in case children vector is modified during iteration]
    std::vector<Entity> childrenCopy = t->children_;
    for (Entity child : childrenCopy)
    {
        if (scene.IsValid(child))
            UpdateRecursive(scene, child, parentDirty);
    }
}

} // namespace TLETC::ECS
