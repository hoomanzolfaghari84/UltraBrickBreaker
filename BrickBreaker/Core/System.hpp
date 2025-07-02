#pragma once

#include <iostream>
#include "ECS.hpp"
#include "Components.hpp"
#include "Collision.hpp"
#include "Events.hpp"

class System {
public:
    virtual void update(EntityManager& em, float dt) = 0;
    virtual ~System() = default;
};


class CollisionSystem : public System {
public:
    CollisionSystem(EventBus& eventBus) : m_EventBus(eventBus) {}

    void update(EntityManager& em, float dt) override {
        std::unordered_map<std::pair<int, int>, std::vector<Entity>, pair_hash> spatialMap;

        // Broad phase: assign entities to grid cells
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!em.hasComponent<Collider>(e)) continue;
            const auto& col = em.getComponent<Collider>(e);

            for (const auto& cell : SpatialHash::getOccupiedCells(col)) {
                spatialMap[cell].push_back(e);
            }
        }

        std::unordered_set<std::pair<Entity, Entity>, pair_hash> testedPairs;

        // Narrow phase: only test pairs within each cell
        for (const auto& [cell, entities] : spatialMap) {
            for (size_t i = 0; i < entities.size(); ++i) {
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    Entity a = entities[i];
                    Entity b = entities[j];

                    if (a > b) std::swap(a, b); // Ensure pair order
                    if (!testedPairs.emplace(std::make_pair(a, b)).second) continue; // Already tested

                    const auto& colA = em.getComponent<Collider>(a);
                    const auto& colB = em.getComponent<Collider>(b);

                    std::optional<CollisionEvent> result;

                    if (colA.type == ColliderShapeType::Circle && colB.type == ColliderShapeType::Circle) {
                        result = circleVsCircle(colA, colB, a, b);
                    }
                    else if (colA.type == ColliderShapeType::Circle && colB.type == ColliderShapeType::Rectangle) {
                        result = circleVsAABB(colA, colB, a, b);
                    }
                    else if (colA.type == ColliderShapeType::Rectangle && colB.type == ColliderShapeType::Circle) {
                        result = circleVsAABB(colB, colA, b, a);
                    }
                    else if (colA.type == ColliderShapeType::Rectangle && colB.type == ColliderShapeType::Rectangle) {
                        result = aabbVsAabb(colA, colB, a, b);
                    }

                    if (result) {
                        std::cout << "Emmiting Collision\n";
                        m_EventBus.emit<CollisionEvent>(*result);
                    }
                }
            }
        }
    }


private:
    EventBus& m_EventBus;
};
