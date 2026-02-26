#include "ScoreSystem.h"
#include "Events/Events.h"

#include "TLETC/ECS/Events/CommonEvents.h"

namespace TLETC::ECS
{

void ScoreSystem::Startup(Scene& scene)
{
    // Subscribe to death events
    events_->Subscribe<EntityDiedEvent>([this](const EntityDiedEvent& e) {
        // Award points for killing enemies
        AddScore(100);
        TLETC_INFO("Score: ", score_);
    });
}

} // namespace TLETC::ECS