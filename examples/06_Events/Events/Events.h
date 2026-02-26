#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/Core/Math.h"

// --- Gameplay Events ---
using namespace TLETC;
using namespace TLETC::ECS;

struct DamageEvent
{
    Entity target;
    Entity source;  // Who caused the damage (can be null)
    float amount;
    vec3 position;
    vec3 direction;
};

struct HealEvent
{
    Entity target;
    float amount;
};

struct EntityDiedEvent
{
    Entity entity;
    Entity killer;  // Can be null
};

struct CollisionEvent
{
    Entity entityA;
    Entity entityB;
    vec3 point;
    vec3 normal;
    float penetration;
};

struct TriggerEnterEvent
{
    Entity trigger;
    Entity other;
};

struct TriggerExitEvent
{
    Entity trigger;
    Entity other;
};