#include "TLETC/ECS/Systems/SystemManager.h"

namespace TLETC::ECS
{

SystemManager::SystemManager()
{
    TLETC_INFO("System Manager Created");
}

SystemManager::~SystemManager()
{
    if(started_)
        Shutdown();
}

void SystemManager::Startup()
{
    TLETC_ASSERT(!started_, "SystemManager already started!");

    SortSystems();

    for (auto& sys : systems_)
    {
        if (sys->IsEnabled())
            sys->Startup(scene_);
    }

    started_ = true;
    TLETC_INFO("SystemManager started with ", systems_.size(), " systems");
}

void SystemManager::Shutdown() 
{
    // Shutdown in reverse order (LIFO)
    for(auto it = systems_.rbegin(); it != systems_.rend(); ++it)
    {
        if((*it)->IsEnabled())
            (*it)->Shutdown(scene_);
    }

    started_ = false;
    TLETC_INFO("SystemManager shut down");
}

void SystemManager::Tick(float frameDt)
{
    TLETC_ASSERT(started_, "Call Startup() before Tick()!");

    // Clamp to prevent Death Spiral
    // (if frame takes >250ms, don't try to catch up physics)
    if(frameDt > ts.maxFrameTime_)
    {
        TLETC_WARN("Frame time ", frameDt*1000.0f, "ms clamped to ", ts.maxFrameTime_*1000.0f, "ms");
        frameDt = ts.maxFrameTime_;
    }

    ts.accumulator_ += frameDt;
    
    // PreUpdate (variable)
    for(auto& sys : systems_)
    {
        if(sys->IsEnabled())
            sys->PreUpdate(scene_, frameDt);
    }

    // FixedUpdate (fixed steps) -> runs multiple times per frame if game is running slowly (0 if frame is faster than fixed steps)
    while(ts.accumulator_ >= ts.fixedDt_)
    {
        for(auto& sys : systems_)
        {
            if(sys->IsEnabled())
                sys->FixedUpdate(scene_, ts.fixedDt_);
        }
        ts.accumulator_ -= ts.fixedDt_;
    }

    // Update (variable)
    for(auto& sys : systems_)
    {
        if(sys->IsEnabled())
            sys->Update(scene_, frameDt);
    }

    // PostUpdate (variable)
    for(auto& sys : systems_)
    {
        if(sys->IsEnabled())
            sys->PostUpdate(scene_, frameDt);
    }
}

void SystemManager::Render()
{
    TLETC_ASSERT(started_, "Call Startup() before Render()!");

    // Alpha: how far we are between the last fixed step and the next
    // 0.0 = exactly at last physics step
    // 1.0 = exactly at next physics step
    //
    // Use this to interpolate render positions:
    // renderPos = prevPos * (1 - alpha) + currPos * alpha
    float alpha = ts.accumulator_ / ts.fixedDt_;

    for (auto& sys : systems_)
        if (sys->IsEnabled())
            sys->PreRender(scene_, alpha);

    for (auto& sys : systems_)
        if (sys->IsEnabled())
            sys->Render(scene_, alpha);

    for (auto& sys : systems_)
        if (sys->IsEnabled())
            sys->PostRender(scene_, alpha);
}

void SystemManager::SortSystems()
{
    std::stable_sort(systems_.begin(), systems_.end(), [](const auto& a, const auto& b) { return a->Priority() < b->Priority(); });
}

} // namespace TLETC::ECS
