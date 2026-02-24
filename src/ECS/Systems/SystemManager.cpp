#include "TLETC/ECS/Systems/SystemManager.h"
#include <algorithm>
#include <queue>

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

bool SystemManager::CreateGroup(const std::string& name) 
{
    if(groups_.find(name) != groups_.end())
        return false;
    groups_[name] = {};
    return true;
}

void SystemManager::SetGroupEnabled(const std::string& groupName, bool enabled)
{
    auto it = groups_.find(groupName);
    if (it == groups_.end())
    {
        TLETC_WARN("Group not found: ", groupName);
        return;
    }

    for (System* sys : it->second)
        sys->SetEnabled(enabled);

    TLETC_INFO("Group '", groupName, "' ", enabled ? "enabled" : "disabled");
}

bool SystemManager::IsGroupEnabled(const std::string& groupName) const
{
    auto it = groups_.find(groupName);
    if (it == groups_.end()) return false;

    // Group is enabled if ANY system in it is enabled
    for (System* sys : it->second)
        if (sys->IsEnabled())
            return true;
    return false;
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

void SystemManager::TopologicalSort()
{
    const size_t N = systems_.size();
    if (N <= 1) return;

    struct Node {
        std::unordered_set<int> out;
        size_t indegree = 0;
        int priority = 0;
    };

    std::vector<Node> graph(N);
    std::unordered_map<std::type_index, int> idFromType;
    idFromType.reserve(N);

    // Assign IDs to systems
    for (int i = 0; i < (int)N; ++i) 
    {
        idFromType.emplace(std::type_index(typeid(*systems_[i])), i);
        graph[i].priority = systems_[i]->Priority();
    }

    // Build edges
    for (int i = 0; i < (int)N; ++i) 
    {
        System* s = systems_[i].get();
        std::type_index self = std::type_index(typeid(*s));

        // RunsBefore: i -> target
        for (auto& t : s->RunsBefore()) 
        {
            auto it = idFromType.find(t);
            if (it == idFromType.end()) continue;
            int j = it->second;
            if (graph[i].out.insert(j).second)
                graph[j].indegree++;
        }

        // RunsAfter: target -> i
        for (auto& t : s->RunsAfter()) 
        {
            auto it = idFromType.find(t);
            if (it == idFromType.end()) continue;
            int j = it->second;
            if (graph[j].out.insert(i).second)
                graph[i].indegree++;
        }
    }

    // Priority queue for deterministic order
    struct QItem {
        int id;
        int priority;
        bool operator<(const QItem& o) const 
        {
            if (priority != o.priority) return priority > o.priority;
            return id > o.id;
        }
    };

    std::priority_queue<QItem> q;

    for (int i = 0; i < (int)N; ++i)
        if (graph[i].indegree == 0)
            q.push({i, graph[i].priority});

    std::vector<int> order;
    order.reserve(N);

    while (!q.empty()) 
    {
        auto [u, prio] = q.top();
        q.pop();
        order.push_back(u);

        for (int v : graph[u].out) 
        {
            if (--graph[v].indegree == 0)
                q.push({v, graph[v].priority});
        }
    }

    // Cycle detection
    if (order.size() != N) // silently fail or log, depending on your design preference
        return;

    // Rebuild systems_ in sorted order
    std::vector<UniquePtr<System>> sorted;
    sorted.reserve(N);

    for (int id : order)
        sorted.push_back(std::move(systems_[id]));

    systems_.swap(sorted);
}

void SystemManager::SortSystems()
{
    std::stable_sort(systems_.begin(), systems_.end(), [](const auto& a, const auto& b) { return a->Priority() < b->Priority(); });
}

} // namespace TLETC::ECS
