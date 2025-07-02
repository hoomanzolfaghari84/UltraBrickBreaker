// PhysicsResponseSystem.hpp
#pragma once
#include "CollisionEvent.hpp"
#include <unordered_map>
#include <SFML/System/Vector2.hpp>


class PhysicsResponseSystem {
public:
    PhysicsResponseSystem(EventBus& bus,
        std::unordered_map<uint32_t, VelocityComponent>& velocities)
        : velocities(velocities)
    {
        bus.Subscribe<CollisionEvent>([this](const CollisionEvent& evt) {
            if (!evt.isTrigger)
                Reflect(evt);
            });
    }

private:
    std::unordered_map<uint32_t, VelocityComponent>& velocities;

    void Reflect(const CollisionEvent& evt) {
        auto reflect = [](sf::Vector2f v, sf::Vector2f n) {
            return v - 2.f * Dot(v, n) * n;
            };

        if (velocities.count(evt.entityA)) {
            auto& v = velocities[evt.entityA].velocity;
            v = reflect(v, evt.contactNormal);
        }

        if (velocities.count(evt.entityB)) {
            auto& v = velocities[evt.entityB].velocity;
            v = reflect(v, -evt.contactNormal); // reverse for B
        }
    }

    float Dot(sf::Vector2f a, sf::Vector2f b) const {
        return a.x * b.x + a.y * b.y;
    }
};
