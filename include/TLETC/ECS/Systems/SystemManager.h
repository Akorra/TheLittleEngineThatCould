#pragma once

#include "TLETC/ECS/Scene.h"
#include "TLETC/ECS/Events/EventBus.h"
#include "TLETC/ECS/Systems/System.h"
#include "TLETC/ECS/Systems/SystemStats.h"
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

    // System Groups
    bool CreateGroup(const std::string& name);

    // Groups need to be created normaly first
    template<typename T>
    void AddSystemToGroup(const std::string& groupName);

    // Enable/disable entire group
    void SetGroupEnabled(const std::string& groupName, bool enabled);
    bool IsGroupEnabled(const std::string& groupName) const;

    // Lifecycle
    void Startup();
    void Shutdown();

    // Per-Frame
    void Tick(float frameDt);
    void Render();

    // Config
    void  SetFixedTimestep(float dt) { ts.fixedDt_ = dt; }
    float GetFixedTimestep() const   { return ts.fixedDt_; }

    // Events
    EventBus& GetEventBus() { return events_; }
    const EventBus& GetEventBus() const { return events_; }

    float GetAlpha() const { return ts.accumulator_ / ts.fixedDt_; }

#ifdef DEBUG 
    const SystemProfiler& GetProfiler() const { return profiler_; }
    void ResetProfiler() { profiler_.Reset(); }
#endif

private:
    void TopologicalSort();
    void SortSystems();

private:
    Scene scene_;
    EventBus events_;

    std::vector<UniquePtr<System>> systems_;
    std::unordered_map<std::string, std::vector<System*>> groups_;

#ifdef DEBUG 
    SystemProfiler profiler_;    
#endif

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

    ref.SetEventBus(&events_); //< EventBus Access 

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

template<typename T>
void SystemManager::AddSystemToGroup(const std::string& groupName)
{
    T* sys = GetSystem<T>();
    TLETC_ASSERT(sys, "System not found!");
    groups_[groupName].push_back(sys);
}

} // namespace TLETC::ECS
