#include "TLETC/ECS/Systems/TransformSystem.h"
#include "TLETC/ECS/Transform.h"
#include "TLETC/ECS/Scene.h"

namespace TLETC::ECS
{
void TransformSystem::FixedUpdate(Scene& scene, float dt)
{
    scene.View<Transform>(
        [&](Entity e, Transform& t)
        {
            if (scene.IsValid(t.parent))
                return; // skip non-roots

            UpdateRecursive(scene, e, false);
        });
}

void TransformSystem::UpdateRecursive(Scene& scene, Entity e, bool parentDirty)
{
    auto* t = scene.GetComponent<Transform>(e);
    if (!t)
        return;

    if (t->dirty || parentDirty)
    {
        t->prevWorldMatrix = t->worldMatrix;

        mat4 local = translate(mat4(1.0f), t->position) //< T
                   * mat4_cast(t->rotation)             //< R
                   * scale(glm::mat4(1.0f), t->scale);  //< S 

        if (scene.IsValid(t->parent))
        {
            auto* parentT  = scene.GetComponent<Transform>(t->parent);
            t->worldMatrix = parentT->worldMatrix * local;
        }
        else
        {
            t->worldMatrix = local;
        }

        t->dirty = false;
    }

    for (auto child : t->children)
        UpdateRecursive(scene, child, t->dirty);
}
} // namespace TLETC::ECS

