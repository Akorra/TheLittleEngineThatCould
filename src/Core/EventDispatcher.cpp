#include "TLETC/Core/EventDispatcher.h"

namespace TLETC 
{
    
EventDispatcher::EventDispatcher() { }

EventDispatcher::~EventDispatcher() { }

void EventDispatcher::Dispatch(Event& event) 
{
    auto it = callbacks_.find(std::type_index(typeid(event)));
    if (it != callbacks_.end()) 
    {
        for (auto& callback : it->second)
        {
            if (event.handled) break;
            callback(event);
        }
    }
}

void EventDispatcher::Clear() 
{
    callbacks_.clear();
}

} // namespace TLETC
