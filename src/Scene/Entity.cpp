#include "TLETC/Scene/Entity.h"

#include "TLETC/Core/Application.h"

#include <algorithm>

namespace TLETC {

Entity::Entity(const std::string& name)
    : name(name)
    , enabled_(true)
    , initialized_(false)
    , input_(nullptr)
    , application_(nullptr)
{
}

Entity::~Entity() 
{
    Destroy();
}

void Entity::RemoveBehaviour(Behaviour* behaviour) 
{
    auto it = std::find_if(behaviours_.begin(), behaviours_.end(), [behaviour](const UniquePtr<Behaviour>& ptr) { return ptr.get() == behaviour; });
    
    if (it != behaviours_.end()) 
    {
        (*it)->OnDestroy();
        behaviours_.erase(it);
    }
}

void Entity::Init() 
{
    if (initialized_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnInit();
    }
    
    initialized_ = true;
}

void Entity::Destroy() 
{
    for (auto& behaviour : behaviours_) 
        behaviour->OnDestroy();

    behaviours_.clear();
    initialized_ = false;
}

bool Entity::RegisterBehaviour(Behaviour* b) const
{
    if(!application_ || !b) 
        return false;

    application_->RegisterBehaviourForEvents(b);

    return true;
}

} // namespace TLETC