#pragma once

#include "TLETC/ECS/Entity.h"

#include <tuple>

namespace TLETC::ECS
{

// Default: no dependencies
template<typename T>
struct ComponentDependencies
{
    using Dependencies = std::tuple<>;
};

// Helper: Apply dependencies recursively
template<typename Scene, typename T>
void EnsureDependencies(Scene& scene, Entity entity)
{
    using Deps = typename ComponentDependencies<T>::Dependencies;
    EnsureDependenciesImpl(scene, entity, Deps{});
}

template<typename Scene, typename... Deps>
void EnsureDependenciesImpl(Scene& scene, Entity entity, std::tuple<Deps...>)
{
    // For each dependency: add if missing (fold expression!)
    (..., [&] {
        if (!scene.template HasComponent<Deps>(entity))
            scene.template AddComponent<Deps>(entity);
    }());
}

} // namespace TLETC::ECS