#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Input.h"
#include "TLETC/Scene/Transform.h"
#include "TLETC/Scene/Behaviour.h"
#include "TLETC/Resources/Mesh.h"

#include <vector>
#include <memory>
#include <string>

namespace TLETC 
{
/**
 * Entity - A game object that can have behaviours attached
 * 
 * Entities represent objects in your game world.
 * They have a transform and can have multiple behaviours attached.
 */
class Entity 
{
public:
    Entity(const std::string& name = "Entity");
    ~Entity();

    // Transform
    Transform transform;

    // Mesh (optional)
    const Mesh* mesh;
    
    // Name
    std::string name;

    // Behaviour management
    template<typename T, typename... Args>
    T* AddBehaviour(Args&&... args) 
    {
        static_assert(std::is_base_of<Behaviour, T>::value, "T must derive from Behaviour");
        
        auto behaviour = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = behaviour.get();
        behaviour->entity_ = this;
        behaviours_.push_back(std::move(behaviour));
        
        // Call OnInit if entity is already initialized
        if (initialized_)
            ptr->OnInit();
        
        return ptr;
    }

    template<typename T>
    T* GetBehaviour() const 
    {
        static_assert(std::is_base_of<Behaviour, T>::value, "T must derive from Behaviour");
        
        for (const auto& behaviour : behaviours_) 
        {
            T* ptr = dynamic_cast<T*>(behaviour.get());
            if (ptr) return ptr;
        }
        return nullptr;
    }

    template<typename T>
    std::vector<T*> GetBehaviours() const 
    {
        static_assert(std::is_base_of<Behaviour, T>::value, "T must derive from Behaviour");
        
        std::vector<T*> result;
        for (const auto& behaviour : behaviours_) 
        {
            T* ptr = dynamic_cast<T*>(behaviour.get());
            if (ptr) result.push_back(ptr);
        }
        return result;
    }

    void RemoveBehaviour(Behaviour* behaviour);
    
    // Lifecycle
    void Init();
    void Destroy();
    
    // Update phases (in order)
    void EarlyUpdate(float deltaTime);
    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    
    // Rendering phases (in order)
    void PreRender();
    void Render();
    void PostRender();
    
    // Event handling
    void OnKeyPressed(KeyCode key);
    void OnKeyReleased(KeyCode key);
    void OnMouseButtonPressed(MouseButton button);
    void OnMouseButtonReleased(MouseButton button);
    void OnMouseMoved(const Vec2& position, const Vec2& delta);
    void OnMouseScrolled(const Vec2& offset);
    
    // Enable/disable
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }
    
    // Input access (can be set by scene/game)
    void SetInput(Input* input) { input_ = input; }
    Input* GetInput() const { return input_; }

private:
    std::vector<UniquePtr<Behaviour>> behaviours_;
    bool enabled_;
    bool initialized_;
    Input* input_;
};

} // namespace TLETC