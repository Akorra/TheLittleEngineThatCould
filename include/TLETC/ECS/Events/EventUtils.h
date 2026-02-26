#pragma once

#include "TLETC/Platform/Time.h"
#include "TLETC/ECS/Events/EventBus.h"

#include <unordered_map>
#include <typeindex>
#include <functional>

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

// Scoped Subscriptions (RAII)
class ScopedSubscription
{
public:
    template<typename T>
    ScopedSubscription(EventBus& bus, std::function<void(const T&)> handler)
        : bus_(&bus)
    {
        id_ = bus.Subscribe<T>(handler);
        typeID_ = std::type_index(typeid(T));
    }
    
    ~ScopedSubscription()
    {
        if (bus_)
            bus_->Unsubscribe(id_);
    }

private:
    EventBus* bus_;
    EventBus::HandlerID id_;
    std::type_index typeID_;
};

} // namespace TLETC::ECS