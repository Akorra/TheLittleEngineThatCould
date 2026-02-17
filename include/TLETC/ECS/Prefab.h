#pragma once

#include "TLETC/ECS/Entity.h"
#include <functional>
#include <vector>

namespace TLETC::ECS
{
class Scene;

/**
 * Prefab (recipe)
 *
 * A template for creating entities.
 * Stores a list of "applicator" functions that add
 * components to a newly created entity.
 */
class Prefab
{
public:
    // Add a default-constructed component
    template<typename T>
    Prefab& AddComponent()
    {
        applicators_.push_back([](Scene& scene, Entity e) {
            scene.AddComponent<T>(e);
        });
        return *this;
    }

    // Add a component with a specific value
    template<typename T>
    Prefab& AddComponent(T value)
    {
        applicators_.push_back([v = std::move(value)](Scene& scene, Entity e) {
            scene.AddComponent<T>(e, v);
        });
        return *this;
    }

    // Add a default-constructed component
    template<typename T, typename... Args>
    Prefab& AddComponent(Args&&... args)
    {
        auto capturedArgs = std::forward_as_tuple(args...);

        applicators_.push_back([argsTuple = std::make_tuple(std::forward<Args>(args)...)](Scene& scene, Entity e) mutable {
            std::apply([&](auto&&... a){ scene.AddComponent<T>(e, std::forward<decltype(a)>(a)...); }, argsTuple);
        });
        return *this;
    }

    // Apply all components to entity
    void Apply(Scene& scene, Entity e) const
    {
        for (auto& applicator : applicators_)
            applicator(scene, e);
    }

private:
    std::vector<std::function<void(Scene&, Entity)>> applicators_;
};

} // namespace TLETC::ECS