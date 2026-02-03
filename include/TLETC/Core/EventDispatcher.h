#pragma once

#include "TLETC/Core/Event.h"

namespace TLETC {

// Event dispatcher - manages event subscriptions and dispatching
class EventDispatcher 
{
public:
    using EventCallback = std::function<void(Event&)>;
    
    EventDispatcher();
    ~EventDispatcher();
    
    // Subscribe to a specific event type
    template<typename T>
    void Subscribe(std::function<void(T&)> callback) 
    {
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");
        
        auto wrapper = [callback](Event& e) 
        {
            callback(static_cast<T&>(e));
        };
        
        callbacks_[std::type_index(typeid(T))].push_back(wrapper);
    }
    
    // Dispatch an event to all subscribers
    void Dispatch(Event& event);
    
    // Clear all subscriptions
    void Clear();
    
private:
    std::unordered_map<std::type_index, std::vector<EventCallback>> callbacks_;
};

// ============================================================================
// Railroad-Themed Aliases
// ============================================================================

// The telegraph sends signals between train components (dispatches events)
using Telegraph = EventDispatcher;

// Alternative aliases
using SignalTower = EventDispatcher;

} // namespace TLETC