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

        return CollisionEvent{
            entityA, entityB,
            a.position + normal * shapeA.radius,
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

    Vec2 delta = circle.position - closest;
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
