#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Log.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>

namespace TLETC::ECS
{

struct SystemStats
{
    std::string name;
    float lastMs    = 0.0f;
    float avgMs     = 0.0f;
    float maxMs     = 0.0f;
    float minMs     = 999.0f;
    uint32 callCount = 0;
};

class SystemProfiler
{
public:
    void BeginSample(const std::string& name)
    {
        samples_[name].start = std::chrono::high_resolution_clock::now();
    }

    void EndSample(const std::string& name)
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto& sample = samples_[name];

        float ms = std::chrono::duration<float, std::milli>(end - sample.start).count();

        sample.sys.lastMs = ms;
        sample.sys.avgMs = (sample.sys.avgMs * sample.sys.callCount + ms) / (sample.sys.callCount + 1);
        sample.sys.maxMs = std::max(sample.sys.maxMs, ms);
        sample.sys.minMs = std::min(sample.sys.minMs, ms);
        sample.sys.callCount++;

        sample.sys.name = name;
    }

    std::vector<SystemStats> GetStats() const
    {
        std::vector<SystemStats> result;
        for (auto& [name, stats] : samples_)
            result.push_back({stats.sys});
        return result;
    }

    void Reset()
    {
        for (auto& [name, stats] : samples_)
        {
            stats.sys.avgMs = 0.0f;
            stats.sys.maxMs = 0.0f;
            stats.sys.minMs = 999.0f;
            stats.sys.callCount = 0;
        }
    }

    void PrintStats() const
    {
        TLETC_INFO("=== System Performance ===");
        for (auto& [name, stats] : samples_)
            TLETC_INFO(name, ": last=", stats.sys.lastMs, "ms avg=", stats.sys.avgMs, "ms max=", stats.sys.maxMs, "ms");
    }

private:
    struct Sample
    {
        std::chrono::high_resolution_clock::time_point start;
        SystemStats sys;
    };

    std::unordered_map<std::string, Sample> samples_;
};

} // namespace TLETC::ECS