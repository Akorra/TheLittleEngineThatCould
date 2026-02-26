#pragma once

#include "TLETC/Platform/Time.h"

#include <unordered_map>
#include <typeindex>

namespace TLETC::ECS
{

// Throttle frequent events
class EventThrottler
{
public:
    bool ShouldFire(std::type_index eventType)
    {
        float now = Time::TotalTime();

        auto it = lastFireTime_.find(eventType);
        if(it == lastFireTime_.end())
        {
            lastFireTime_[eventType] = now;
            return true;
        }
        
        if (now - it->second < minInterval_)
            return false;
        
        lastFireTime_[eventType] = now;
        return true;
    }

private:
    float minInterval_ = 0.1f;  // Max 10 events/second
    std::unordered_map<std::type_index, float> lastFireTime_;
};

} // namespace TLETC::ECS