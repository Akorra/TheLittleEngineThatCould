#pragma once

#include "TLETC/ECS/Systems/System.h"

namespace TLETC::ECS
{

class ScoreSystem : public System
{
public:
    const char* Name() const override { return "ScoreSystem"; }
    int Priority() const override { return 100; }
    
    void Startup(Scene& scene) override;
    
    int GetScore() const { return score_; }
    void AddScore(int points) { score_ += points; }

private:
    int score_ = 0;
};

} // namespace TLETC::ECS