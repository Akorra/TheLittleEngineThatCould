#pragma once

#include "TLETC/ECS/Entity.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Core/Serialization/BinaryWriter.h"

#include <typeindex>

namespace TLETC::ECS
{
// --- Entity Lifecycle ---
struct EntityCreatedEvent   
{ 
    Entity entity; 

    std::vector<uint8> Serialize() const
    {
        BinaryWriter writer;
        writer.Write(entity.id);
        return writer.GetBuffer();
    }

    // Deserialize from bytes
    static EntityCreatedEvent Deserialize(const std::vector<uint8>& data)
    {
        BinaryReader reader(data);
        
        EntityCreatedEvent event;
        event.entity.id = reader.Read<uint64>();
    
        return event;
    }
};

struct EntityDestroyedEvent 
{ 
    Entity entity; 

    std::vector<uint8> Serialize() const
    {
        BinaryWriter writer;
        writer.Write(entity.id);
        return writer.GetBuffer();
    }

    // Deserialize from bytes
    static EntityDestroyedEvent Deserialize(const std::vector<uint8>& data)
    {
        BinaryReader reader(data);
        
        EntityDestroyedEvent event;
        event.entity.id = reader.Read<uint64>();
    
        return event;
    }
};

// --- Component Lifecycle ---
template<typename T>
struct ComponentAddedEvent   { Entity entity; T* component; };

template<typename T>
struct ComponentRemovedEvent { Entity entity; std::type_index component{ typeid(T) }; };
};