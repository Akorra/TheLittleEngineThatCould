#include "TLETC/Scene/Entity.h"
#include <algorithm>

namespace TLETC {

Entity::Entity(const std::string& name)
    : name(name)
    , mesh(nullptr)
    , enabled_(true)
    , initialized_(false)
    , input_(nullptr)
{
}

Entity::~Entity() 
{
    Destroy();
}

void Entity::RemoveBehaviour(Behaviour* behaviour) 
{
    auto it = std::find_if(behaviours_.begin(), behaviours_.end(),
        [behaviour](const UniquePtr<Behaviour>& ptr) 
        {
            return ptr.get() == behaviour;
        });
    
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

void Entity::EarlyUpdate(float deltaTime) 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnEarlyUpdate(deltaTime);
    }
}

void Entity::Update(float deltaTime) 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnUpdate(deltaTime);
    }
}

void Entity::LateUpdate(float deltaTime) 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_)
    {
        if (behaviour->IsEnabled())
            behaviour->OnLateUpdate(deltaTime);
    }
}

void Entity::Destroy() 
{
    for (auto& behaviour : behaviours_) 
        behaviour->OnDestroy();

    behaviours_.clear();
    initialized_ = false;
}

void Entity::PreRender() 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnPreRender();
    }
}

void Entity::Render() 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnRender();
    }
}

void Entity::PostRender() 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnPostRender();
    }
}

void Entity::OnKeyPressed(KeyCode key) 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnKeyPressed(key);
    }
}

void Entity::OnKeyReleased(KeyCode key) 
{
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnKeyReleased(key);
    }
}

void Entity::OnMouseButtonPressed(MouseButton button) {
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnMouseButtonPressed(button);
    }
}

void Entity::OnMouseButtonReleased(MouseButton button) {
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled()) 
            behaviour->OnMouseButtonReleased(button);
    }
}

void Entity::OnMouseMoved(const Vec2& position, const Vec2& delta) {
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled()) 
            behaviour->OnMouseMoved(position, delta);
    }
}

void Entity::OnMouseScrolled(const Vec2& offset) {
    if (!enabled_) return;
    
    for (auto& behaviour : behaviours_) 
    {
        if (behaviour->IsEnabled())
            behaviour->OnMouseScrolled(offset);
    }
}

} // namespace TLETC