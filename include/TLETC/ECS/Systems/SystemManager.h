#pragma once

#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/Systems/System.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Core/Log.h"

namespace TLETC::ECS
{
/**
 * SystemManager
 * 
 * Owns all systems.
 * Calls systems in priority order, in each phase.
 * Handles fixed timestep accumulation.
 */
class SystemManager
{
public:
    SystemManager();
    ~SystemManager();

    // Scene Access
    Scene& GetScene() { return scene_; }
    const Scene& GetScene() const { return scene_; }

    // Register System:
    template<typename T, typename... Args>
    T& AddSystem(Args&&... args);

    template<typename T>
    T* GetSystem();

    template<typename T>
    void RemoveSystem();

    // Lifecycle
    void Startup();
    void Shutdown();

    // Per-Frame
    void Tick(float frameDt);
    void Render();

    // Config
    void  SetFixedTimestep(float dt) { ts.fixedDt_ = dt; }
    float GetFixedTimestep() const   { return ts.fixedDt_; }

    float GetAlpha() const { return ts.accumulator_ / ts.fixedDt_; }

private:
    void SortSystems();

private:
    Scene scene_;

    std::vector<UniquePtr<System>> systems_;

    struct TimestepState {
        float fixedDt_      = 1.0f / 60.0f;
        float accumulator_  = 0.0f;
        float maxFrameTime_ = 0.25f; // prevent death spiral
    } ts;

    bool started_ = false;
};

// Template Implementation
template<typename T, typename... Args>
T& SystemManager::AddSystem(Args&&... args)
{
    static_assert(std::is_base_of_v<System, T>, "T must Derive from System.");

    // Chack for duplicate
    TLETC_ASSERT(!GetSystem<T>(), "System already registered!");

    auto system = MakeUnique<T>(std::forward<Args>(args)...);
    T& ref = *system;

    systems_.push_back(std::move(system));

    SortSystems();

    // If we're already running, start imediately
    if(started_)
        ref.Startup(scene_);
    
    TLETC_INFO("Registered system: ", ref.Name(), " (priority=", ref.Priority(), ")");
    return ref;
}

template<typename T>
T* SystemManager::GetSystem() 
{
    for (auto& sys : systems_)
    {
        if (T* typed = dynamic_cast<T*>(sys.get()))
            return typed;
    }
    return nullptr;
}

template<typename T>
void SystemManager::RemoveSystem()
{
    auto it = std::find_if(systems_.begin(), systems_.end(), [](const auto& s) { return dynamic_cast<T*>(s.get()) != nullptr; });

    if (it != systems_.end())
    {
        (*it)->Shutdown(scene_);
        systems_.erase(it);
    }
}

} // namespace TLETC::ECS
