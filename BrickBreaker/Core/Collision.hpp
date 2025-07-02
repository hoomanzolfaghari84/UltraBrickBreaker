#pragma once

#include <optional>
#include <cmath>


#include "ECS.hpp"
#include "Components.hpp"
#include "Events.hpp"


inline std::optional<CollisionEvent> circleVsCircle(const Collider& a, const Collider& b, Entity entityA, Entity entityB) {
    const auto& shapeA = std::get<CircleCollisionShape>(a.shape);
    const auto& shapeB = std::get<CircleCollisionShape>(b.shape);

    Vec2 delta = b.position - a.position;
    float distSq = delta.lengthSquared();
    float radiusSum = shapeA.radius + shapeB.radius;

    if (distSq < radiusSum* radiusSum) {
        float dist = std::sqrt(distSq);
        Vec2 normal = dist > 0 ? delta * 1/dist : Vec2{ 1, 0 };
        float penetration = radiusSum - dist;
        
        Vec2 contactPoint = a.position + normal * (shapeA.radius - 0.5f * penetration);


        return CollisionEvent{
            entityA, entityB,
            //a.position + normal * shapeA.radius,
            contactPoint,
            normal,
            normal * penetration,
            penetration,
            a.isTrigger || b.isTrigger
        };
    }
    return std::nullopt;
}

inline std::optional<CollisionEvent> circleVsAABB(const Collider& circle, const Collider& rect, Entity entityCircle, Entity entityRect) {
    const auto& cShape = std::get<CircleCollisionShape>(circle.shape);
    const auto& rShape = std::get<RectangleCollisionShape>(rect.shape);

    Vec2 relCenter = circle.position - rect.position;
    Vec2 clamped = relCenter.clamp(-rShape.halfsize, rShape.halfsize);
    Vec2 closest = rect.position + clamped;

    Vec2 delta = closest - circle.position;
    float distSq = delta.lengthSquared();

    if (distSq < cShape.radius*cShape.radius) {
        float dist = std::sqrt(distSq);
        Vec2 normal = dist > 0 ? delta / dist : Vec2{ 1, 0 };
        float penetration = cShape.radius - dist;

        return CollisionEvent{
            entityCircle, entityRect,
            closest,
            normal,
            normal * penetration,
            penetration,
            circle.isTrigger || rect.isTrigger
        };
    }
    return std::nullopt;
}


inline std::optional<CollisionEvent> aabbVsAabb(const Collider& a, const Collider& b, Entity entityA, Entity entityB) {
    const auto& shapeA = std::get<RectangleCollisionShape>(a.shape);
    const auto& shapeB = std::get<RectangleCollisionShape>(b.shape);

    // Get min and max points
    Vec2 aMin = a.position - shapeA.halfsize;
    Vec2 aMax = a.position + shapeA.halfsize;
    Vec2 bMin = b.position - shapeB.halfsize;
    Vec2 bMax = b.position + shapeB.halfsize;

    bool overlapX = aMin.x <= bMax.x && aMax.x >= bMin.x;
    bool overlapY = aMin.y <= bMax.y && aMax.y >= bMin.y;

    if (overlapX && overlapY) {
        // Calculate penetration depth
        float dx = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
        float dy = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);

        Vec2 normal;
        float penetration;

        Vec2 direction = (b.position - a.position);
        normal = (dx < dy)
            ? Vec2{ (direction.x < 0 ? -1.f : 1.f), 0.f }
        : Vec2{ 0.f, (direction.y < 0 ? -1.f : 1.f) };

        penetration = dx < dy ? dx : dy;

        Vec2 contactPoint = (a.position + b.position) * 0.5f;

        return CollisionEvent{
            entityA, entityB,
            contactPoint,
            normal,
            normal * penetration,
            penetration,
            a.isTrigger || b.isTrigger
        };
    }

    return std::nullopt;
}




struct SpatialHash {
    static constexpr float CELL_SIZE = 100.f; // Tune this per your game scale

    static std::pair<int, int> getCell(const Vec2& pos) {
        return {
            static_cast<int>(std::floor(pos.x / CELL_SIZE)),
            static_cast<int>(std::floor(pos.y / CELL_SIZE))
        };
    }

    static std::vector<std::pair<int, int>> getOccupiedCells(const Collider& collider) {
        std::vector<std::pair<int, int>> cells;

        Vec2 min, max;

        if (collider.type == ColliderShapeType::Circle) {
            const auto& c = std::get<CircleCollisionShape>(collider.shape);
            min = collider.position - Vec2{ c.radius, c.radius };
            max = collider.position + Vec2{ c.radius, c.radius };
        }
        else if (collider.type == ColliderShapeType::Rectangle) {
            const auto& r = std::get<RectangleCollisionShape>(collider.shape);
            min = collider.position - r.halfsize;
            max = collider.position + r.halfsize;
        }
        else {
            // Extend for VertexArray if needed
            return {};
        }

        auto minCell = getCell(min);
        auto maxCell = getCell(max);

        for (int x = minCell.first; x <= maxCell.first; ++x) {
            for (int y = minCell.second; y <= maxCell.second; ++y) {
                cells.emplace_back(x, y);
            }
        }

        return cells;
    }
};


struct pair_hash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        std::hash<T1> h1;
        std::hash<T2> h2;
        return h1(p.first) ^ (h2(p.second) << 1);
    }
};



