#pragma once
#include "ECS.hpp"
#include "Components.hpp"
#include "System.hpp"
#include "Events.hpp"
#include "EventBus.hpp"
#include <unordered_map>
#include <iostream>

class PhysicsSystem : public System {
public:

    PhysicsSystem(EventBus& eventBus) {
        eventBus.subscribe<CollisionEvent>([this](const CollisionEvent& ev) {
            std::cout << "Listened to Collision\n";
            m_CollisionEvents.push_back(ev);
            });
    }

    void update(EntityManager& em, float dt) override {
        // Step 1: Integrate forces (basic Euler)
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!em.hasComponent<Kinematics>(e)) continue;

            auto& kin = em.getComponent<Kinematics>(e);
            if (kin.isStatic) continue;

            kin.velocity += kin.acceleration * dt;
            kin.position += kin.velocity * dt;

            syncPosition(em, e, kin.position);
        }

        // Step 2: Resolve collisions (multiple passes if needed)
        const int resolutionPasses = 3;
        for (int pass = 0; pass < resolutionPasses; ++pass) {
            for (const auto& ev : m_CollisionEvents) {
                if (ev.isTrigger) continue;

                if (!em.hasComponent<Kinematics>(ev.entityA) ||
                    !em.hasComponent<Kinematics>(ev.entityB)) continue;

                auto& a = em.getComponent<Kinematics>(ev.entityA);
                auto& b = em.getComponent<Kinematics>(ev.entityB);

                resolveCollision(a, b, ev.contactNormal, ev.contactPoint);
                positionalCorrection(em, ev.entityA, a, ev.entityB, b, ev.penetrationVector);



            }
        }

        m_CollisionEvents.clear(); // Clear for next frame
    }

private:
    std::vector<CollisionEvent> m_CollisionEvents;

    void resolveCollision(Kinematics& a, Kinematics& b, const Vec2& normalInput, const Vec2& contactPoint) {
        Vec2 normal = normalInput.normalized();  // Ensure unit vector
        float restitution = 1.f; // Perfectly elastic (bounce)

        float invMassA = a.isStatic ? 0.f : a.GetInvMass();
        float invMassB = b.isStatic ? 0.f : b.GetInvMass();

        Vec2 relativeVel = b.velocity - a.velocity;

        float velAlongNormal = Vec2::Dot(relativeVel, normal);

        // If objects are separating, don't resolve
        if (velAlongNormal > 0) return;

        float invMassSum = invMassA + invMassB;
        if (invMassSum == 0.f) return;

        // Calculate impulse scalar
        float j = -(1 + restitution) * velAlongNormal;
        j /= invMassSum;

        Vec2 impulse = j * normal;

        if (!a.isStatic) a.velocity -= impulse * invMassA;
        if (!b.isStatic) b.velocity += impulse * invMassB;

        // DEBUG:
        std::cout << "Applied impulse: " << impulse.x << ", " << impulse.y << "\n";
        std::cout << "New velocities: A(" << a.velocity.x << ", " << a.velocity.y
            << "), B(" << b.velocity.x << ", " << b.velocity.y << ")\n";
    }

    void positionalCorrection(EntityManager& em, Entity ea, Kinematics& a, Entity eb, Kinematics& b, const Vec2& mtv) {
        const float percent = 0.8f;
        const float slop = 0.01f;

        float mtvLen = mtv.length();
        if (mtvLen < slop) return;

        float invMassA = a.isStatic ? 0.f : a.GetInvMass();
        float invMassB = b.isStatic ? 0.f : b.GetInvMass();
        float invMassSum = invMassA + invMassB;

        if (invMassSum == 0.f) return;

        Vec2 normal = mtv / mtvLen;
        float penetration = mtvLen - slop;
        Vec2 correction;

        if (invMassA == 0.f) {
            // Only B moves
            correction = normal * (penetration * percent);
            b.position += correction;
            syncPosition(em, eb, b.position);
        }
        else if (invMassB == 0.f) {
            // Only A moves
            correction = normal * (penetration * percent);
            a.position -= correction;
            syncPosition(em, ea, a.position);
        }
        else {
            // Both move proportionally to their mass
            correction = normal * (penetration * percent / invMassSum);
            a.position -= correction * invMassA;
            b.position += correction * invMassB;
            syncPosition(em, ea, a.position);
            syncPosition(em, eb, b.position);
        }
    }

    void syncPosition(EntityManager& em, Entity e, const Vec2& pos) {
        if (em.hasComponent<Collider>(e)) {
            em.getComponent<Collider>(e).position = pos;
        }
        if (em.hasComponent<RenderRectangle>(e)) {
            em.getComponent<RenderRectangle>(e).position = pos;
        }
        if (em.hasComponent<RenderCircle>(e)) {
            em.getComponent<RenderCircle>(e).position = pos;
        }
    }
};

