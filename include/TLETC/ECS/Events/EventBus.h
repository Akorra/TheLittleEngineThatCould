#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Assert.h"
#include "TLETC/Platform/Time.h"
#include "TLETC/ECS/Events/EventRecorder.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>

namespace TLETC::ECS
{
/**
 * EventBus
 *
 * Type-safe publish-subscribe event system.
 * Systems publish events, other systems subscribe.
 *
 * Thread-safety: NOT thread-safe! Use from one thread only.
 */
class EventBus 
{
public:
    using HandlerID = uint64;
    // Subscribe to an event type
    template<typename T>
    HandlerID Subscribe(std::function<void(const T&)> handler, int priority = 0, std::function<bool(const T&)> ignoreIf = [](const T& event){ return false; })
    {
        std::type_index typeID = std::type_index(typeid(T));
        
        HandlerID id = nextHandlerID_++;
        
        // Type-erase the handler
        auto wrapper = [ignoreIf, handler](const void* data) {
            const T& event = *static_cast<const T*>(data);

            if(ignoreIf(event)) 
                return;

            handler(event);
        };
        
        handlers_[typeID].push_back({id, priority, wrapper});

        SortHandlers(typeID);
        
        return id;
    }

    // Unsubscribe a specific handler
    template<typename T>
    void Unsubscribe(HandlerID id)
    {
        std::type_index typeID = std::type_index(typeid(T));
        
        auto it = handlers_.find(typeID);
        if (it == handlers_.end())
            return;
        
        auto& handlerList = it->second;
        handlerList.erase(
            std::remove_if(handlerList.begin(), handlerList.end(),
                [id](const Handler& h) { return h.id == id; }),
            handlerList.end()
        );
    }

    // Publish an event (calls all subscribers immediately)
    template<typename T>
    void Publish(const T& event)
    {
        // Record if enabled
        if (recorder_) recorder_->Record(event, Time::TotalTime());

        std::type_index typeID = std::type_index(typeid(T));
        
        auto it = handlers_.find(typeID);
        if (it == handlers_.end())
            return;
        
        // Call all handlers
        for (auto& handler : it->second)
            handler.callback(&event);
    }

    // Deferred publish (queued, processed later)
    template<typename T>
    void PublishDeferred(const T& event)
    {
        // Store a copy of the event
        std::type_index typeID = std::type_index(typeid(T));
        
        auto eventCopy = std::make_shared<T>(event);
        
        deferredEvents_.push_back([this, typeID, eventCopy]() {
            auto it = handlers_.find(typeID);
            if (it != handlers_.end())
            {
                for (auto& handler : it->second)
                    handler.callback(eventCopy.get());
            }
        });
    }

    // Process all deferred events
    void ProcessDeferred()
    {
        // Swap to avoid issues if handlers publish more events
        std::vector<std::function<void()>> events;
        events.swap(deferredEvents_);
        
        for (auto& event : events)
            event();
    }

    // Clear all subscriptions
    void Clear()
    {
        handlers_.clear();
        deferredEvents_.clear();
    }

    // Get subscription count for debugging
    size_t GetSubscriberCount() const
    {
        size_t count = 0;
        for (auto& [type, handlerList] : handlers_)
            count += handlerList.size();
        return count;
    }

    // Record Events
    void SetRecorder(EventRecorder* recorder) { recorder_ = recorder; }

private:
    void SortHandlers(std::type_index typeID)
    {
        auto it = handlers_.find(typeID);
        if (it == handlers_.end())
            return;
        
        std::sort(it->second.begin(), it->second.end(),
            [](const Handler& a, const Handler& b) {
                return a.priority < b.priority;  // Lower priority first
            });
    }

private:
    struct Handler
    {
        HandlerID id;
        int       priority = 0;
        std::function<void(const void*)> callback;
    };

    std::unordered_map<std::type_index, std::vector<Handler>> handlers_;
    std::vector<std::function<void()>> deferredEvents_;
    
    HandlerID nextHandlerID_ = 1;

    EventRecorder* recorder_ = nullptr;
};
} // namespace TLETC::ECS
