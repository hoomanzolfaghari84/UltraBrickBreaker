#pragma once

#include "ECS.hpp"
#include "Components.hpp"
#include "Collision.hpp"
#include "Events.hpp"

class System {
public:
    virtual void update(EntityManager& em, float dt) = 0;
    virtual ~System() = default;
};

class MovementSystem : public System {
public:
    void update(EntityManager& em, float dt) override {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (em.hasComponent<Kinematics>(e)) {
                auto& kin = em.getComponent<Kinematics>(e);
                kin.position += kin.velocity * dt;
                kin.velocity += kin.acceleration * dt; // TODO could be more realistic maybe ?? XD
            }
        }
    }
};

class CollisionSystem : public System {
public:
    CollisionSystem(EventBus& eventBus) : m_EventBus(eventBus) {}

    void update(EntityManager& em, float dt) override {
        //ComponentStorage<Collider>& colliders = em.getComponents<Collider>();

        for (Entity entityA = 0; entityA < MAX_ENTITIES; ++entityA) {
            for (Entity entityB = entityA + 1; entityB < MAX_ENTITIES; ++entityB) {
                if (!em.hasComponent<Collider>(entityA) || !em.hasComponent<Collider>(entityB)) continue;
                const auto& colA = em.getComponent<Collider>(entityA);
                const auto& colB = em.getComponent<Collider>(entityB);

                std::optional<CollisionEvent> result;

                if (colA.type == ColliderShapeType::Circle && colB.type == ColliderShapeType::Circle) {
                    result = circleVsCircle(colA, colB, entityA, entityB);
                }
                else if (colA.type == ColliderShapeType::Circle && colB.type == ColliderShapeType::Rectangle) {
                    result = circleVsAABB(colA, colB, entityA, entityB);
                }
                else if (colA.type == ColliderShapeType::Rectangle && colB.type == ColliderShapeType::Circle) {
                    result = circleVsAABB(colB, colA, entityB, entityA);
                }

                if (result) {
                    m_EventBus.emit<CollisionEvent>(*result);
                }
            }
        }

    }

private:
    EventBus& m_EventBus;
};
