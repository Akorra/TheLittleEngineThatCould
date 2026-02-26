#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/Core/Math.h"

#include <typeindex>

namespace TLETC::ECS
{
// --- Entity Lifecycle ---
struct EntityCreatedEvent   { Entity entity; };
struct EntityDestroyedEvent { Entity entity; };

// --- Component Lifecycle ---
template<typename t>
struct ComponentAddedEvent   { Entity entity; T* component; };
template<typename t>
struct ComponentRemovedEvent { Entity entity; std::type_index component{ typeid(T) }; };
};