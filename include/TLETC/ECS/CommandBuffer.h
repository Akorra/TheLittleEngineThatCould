#pragma once

#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/ComponentRegistry.h"
#include <vector>
#include <functional>

namespace TLETC::ECS
{

class Scene;

/**
 * CommandBuffer
 *
 * Records structural changes (add/remove entities/components)
 * and applies them all at once later.
 *
 * This prevents iterator invalidation during View() iteration.
 */
class CommandBuffer
{
public:
    // Record commands
    void DestroyEntity(Entity entity)
    {
        commands_.push_back([entity](Scene& scene) {
            scene.DestroyEntity(entity);
        });
    }

    Entity CreateEntity()
    {
        // Can't return entity immediately (it doesn't exist yet!)
        // Solution: return a "placeholder" that gets resolved on flush

        // For now, simplified: just record creation
        Entity placeholder = Entity::Null();
        commands_.push_back([&placeholder](Scene& scene) {
            placeholder = scene.CreateEntity();
        });

        return placeholder;  // ⚠️ Careful: this is NULL until Flush!
    }

    template<typename T, typename... Args>
    void AddComponent(Entity entity, Args&&... args)
    {
        // Capture args by value
        commands_.push_back(
            [entity, ...capturedArgs = std::forward<Args>(args)](Scene& scene) mutable {
                scene.AddComponent<T>(entity, std::forward<Args>(capturedArgs)...);
            }
        );
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        commands_.push_back([entity](Scene& scene) {
            scene.RemoveComponent<T>(entity);
        });
    }

    // Execute all recorded commands
    void Flush(Scene& scene)
    {
        for (auto& cmd : commands_)
            cmd(scene);

        commands_.clear();
    }

    void Clear()
    {
        commands_.clear();
    }

    bool IsEmpty() const { return commands_.empty(); }
    size_t Size() const { return commands_.size(); }

private:
    std::vector<std::function<void(Scene&)>> commands_;
};

} // namespace TLETC::ECS