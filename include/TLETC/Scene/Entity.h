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

// forward declaration
class Application;

/**
 * Entity - A game object that can have behaviours attached
 * 
 * Entities represent objects in your game world.
 * They have a transform and can have multiple behaviours attached.
 */
class Entity 
{
    friend class Application;
public:
    Entity(const std::string& name = "Entity");
    ~Entity();

    // Transform
    Transform transform;
    
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

        // Register with Application's event system (if we have app reference)
        RegisterBehaviour(ptr);
        
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
    
    // Enable/disable
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }
    
    // Input access (can be set by scene/game)
    void SetInput(Input* input) { input_ = input; }
    Input* GetInput() const { return input_; }

    // Application access (for registering behaviours)
    void SetApplication(class Application* app) { application_ = app; }
    class Application* GetApplication() const { return application_; }

private:
    // Register Behaviours for events
    bool RegisterBehaviour(Behaviour* b) const;

private:
    std::vector<UniquePtr<Behaviour>> behaviours_;
    bool    enabled_;
    bool    initialized_;
    Input*  input_;
    Application* application_;
};

// ============================================================================
// Railroad-Themed Aliases
// ============================================================================

// The telegraph sends signals between train components (dispatches events)
using Wagon = Entity;

} // namespace TLETC