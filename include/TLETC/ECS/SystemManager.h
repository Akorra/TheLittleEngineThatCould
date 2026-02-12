#pragma once

#include "TLETC/ECS/Scene.h"
#include <array>

namespace TLETC::ECS
{

class SystemManager
{
public:
    Scene& GetScene() { return scene_; }

    template<typename T, typename... Args>
    T& AddSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<System, T>);
        
        auto system = UniquePtr<T>(std::forward<Args>(args)...);
        
        T& ref = *system;
        systems_.push_back(std::move(system));
        return ref;
    }

private:
    void SortSystems();
    void Tick(float frameDt);
    void Render();

private:
    Scene scene_;
    std::array<std::vector<System*>, 8> phaseBuckets_;
    std::vector<UniquePtr<System>>      systems_;

    float fixedDt_ = 1.0f / 60.0f;
    float accumulator_ = 0.0f;
    float maxFrameTime_ = 0.25f; // safety clamp
};

// Railroad theme Aliases
using Tracks = SystemManager;
using World  = SystemManager;

} // namespace TLETC::ECS
