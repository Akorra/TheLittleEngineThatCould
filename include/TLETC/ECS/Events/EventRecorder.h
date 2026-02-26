#pragma once

#include "TLETC/Core/Types.h"
#include <vector>
#include <memory>
#include <typeindex>
#include <functional>

namespace TLETC::ECS
{
class EventRecorder
{
public:
    struct RecordedEvent
    {
        std::type_index typeID;
        float timestamp;
        std::shared_ptr<void> data;  // Type-erased event

        RecordedEvent(std::type_index id, float time, std::shared_ptr<void> d) : typeID(id), timestamp(time), data(std::move(d)) {}
        
        template<typename T>
        const T* As() const
        {
            if (typeID != std::type_index(typeid(T)))
                return nullptr;
            return static_cast<const T*>(data.get());
        }
    };

    // Enable/disable recording
    void SetRecording(bool enabled) { recording_ = enabled; }
    bool IsRecording() const { return recording_; }

    // Record an event
    template<typename T>
    void Record(const T& event, float timestamp)
    {
        if (!recording_)
            return;
        
        auto copy = std::make_shared<T>(event);
        
        history_.emplace_back(std::type_index(typeid(T)), timestamp, copy);
    }

    // Get all events of a specific type
    template<typename T>
    std::vector<const T*> GetHistory() const
    {
        std::vector<const T*> result;
        
        for (auto& event : history_)
        {
            if (const T* typed = event.As<T>())
                result.push_back(typed);
        }
        
        return result;
    }

    // Get all events
    const std::vector<RecordedEvent>& GetAllHistory() const
    {
        return history_;
    }

    // Replay events
    void Replay(std::function<void(const RecordedEvent&)> callback) const
    {
        for (auto& event : history_)
            callback(event);
    }

    // Clear history
    void Clear()
    {
        history_.clear();
    }

    size_t GetEventCount() const { return history_.size(); }

private:
    bool recording_ = false;
    std::vector<RecordedEvent> history_;
};

} // namespace TLETC::ECS
