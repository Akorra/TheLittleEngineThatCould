#pragma once

#include "TLETC/ECS/Events/EventBus.h"

namespace TLETC::Network
{

enum class EventID : uint16
{
    Damage = 1,
    Heal = 2,
    EntityDied = 3,
    // ... more events
};

class NetworkEventBus
{
public:
    // Send event over network
    template<typename T>
    void SendEvent(const T& event, EventID id)
    {
        // Serialize
        std::vector<uint8> data = event.Serialize();
        
        // Prepend event ID
        std::vector<uint8> packet;
        packet.resize(sizeof(EventID) + data.size());
        
        std::memcpy(packet.data(), &id, sizeof(EventID));
        std::memcpy(packet.data() + sizeof(EventID), data.data(), data.size());
        
        // Send packet (pseudo-code)
        // networkSocket.Send(packet);
    }

    // Receive and dispatch event
    void ReceivePacket(const std::vector<uint8>& packet, ECS::EventBus& eventBus)
    {
        if (packet.size() < sizeof(EventID))
            return;
        
        EventID id;
        std::memcpy(&id, packet.data(), sizeof(EventID));
        
        std::vector<uint8> data(
            packet.begin() + sizeof(EventID),
            packet.end()
        );
        
        // Deserialize and publish
        /*switch (id)
        {
            case EventID::Damage:
            {
                DamageEvent event = DamageEvent::Deserialize(data);
                eventBus.Publish(event);
                break;
            }
            // ... other events
        }/**/
    }
};

} // namespace TLETC::Network