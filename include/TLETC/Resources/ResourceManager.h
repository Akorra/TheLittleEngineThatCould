#pragma once

#include <unordered_map>
#include <unordered_set>

#include "TLETC/Rendering/Handle.h"

namespace TLETC 
{
template<typename T, typename HandleT>
class ResourceManager 
{
public:
    HandleT Create(T&& resource) 
    {
        uint32 id = ++nextId_;
        resources_.emplace(id, std::move(resource));
        return HandleT(id);
    }

    template<typename... Args>
    HandleT Emplace(Args&&... args) 
    {
        uint32 id = ++nextId_;
        resources_.emplace(id, T(std::forward<Args>(args)...));
        return HandleT(id);
    }

    bool HasHandle(HandleT handle)
    {
        assert(handle.IsValid());
        return resources_.contains(handle.GetID());
    }

    T& Get(HandleT handle) 
    {
        assert(handle.IsValid());
        return resources_[handle.GetID()];
    }

    const T& Get(HandleT handle) const 
    {
        assert(handle.IsValid());
        return resources_.at(handle.GetID());
    }

    void Destroy(HandleT handle) 
    {
        if(!handle.IsValid()) return;
        if (!resources_.contains(handle.GetID())) return;

        toDestroy_.insert(handle.GetID());
    }

    void ForEachResource(std::function<void(HandleT&)> func)
    {
        for(const auto& [key, _] : resources_)
            func(HandleT(key));    
    }

    void ForEachQueuedDestroy(std::function<void(HandleT&)> func)
    {
        for(const uint32 key : toDestroy_)
            func(HandleT(key));
    }

    void ProcessDestroyQueue() 
    {
        for(const uint32 key : toDestroy_)
            resources_.erase(key);
        toDestroy_.clear();
    }

    void Clear() 
    {
        toDestroy_.clear();
        resources_.clear();
    }

private:
    uint32 nextId_ = 0;
    std::unordered_map<uint32, T> resources_;
    std::unordered_set<uint32>    toDestroy_;
};

} // namespace TLETC