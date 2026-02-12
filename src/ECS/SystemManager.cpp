#include "TLETC/ECS/SystemManager.h"
#include "TLETC/ECS/System.h"

#include <algorithm>

namespace TLETC::ECS
{

void SystemManager::SortSystems()
{
    std::sort(systems_.begin(), systems_.end(),
        [](const auto& a, const auto& b)
        {
            return a->GetPriority() < b->GetPriority();
        });
}

void SystemManager::Tick(float frameDt)
{
    // Prevent death spiral
    if (frameDt > maxFrameTime_)
        frameDt = maxFrameTime_;

    accumulator_ += frameDt;

    // Variable pre-update
    for (auto& sys : systems_)
        sys->PreUpdate(scene_, frameDt);

    // Fixed simulation loop
    while (accumulator_ >= fixedDt_)
    {
        for (auto& sys : systems_)
            sys->FixedUpdate(scene_, fixedDt_);
        accumulator_ -= fixedDt_;
    }

    // Variable update
    for (auto& sys : systems_)
        sys->Update(scene_, frameDt);

    for (auto& sys : systems_)
        sys->PostUpdate(scene_, frameDt);
}

void SystemManager::Render()
{
    // Interpolation Alpha [0, 1] : interpolate between prev physics state, current physics state
    // without this with physics running at diferent rate from rendering, we get visible stutter 
    // with interpolation renderPosition = currentPosition * alpha + previousPosition * (1 - alpha);
    // we get smooth transition even when render FPS > physics FPS
    float alpha = accumulator_ / fixedDt_; 

    for(auto& sys : systems_)
        sys->PreRender(scene_, alpha);

    for(auto& sys : systems_)
        sys->Render(scene_, alpha);
 
    for(auto& sys : systems_)
        sys->PostRender(scene_, alpha);
}

} // namespace TLETC::ECS
