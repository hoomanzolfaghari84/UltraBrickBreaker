#pragma once

#include <optional>

#include "ECS.hpp"
#include "../Utils/Vec2.hpp"


struct CollisionEvent {
    Entity entityA;
    Entity entityB;

    Vec2 contactPoint;       // Approximate point of collision
    Vec2 contactNormal;      // Normal vector (unit vector perpendicular to surface)
    Vec2 penetrationVector;  // MTV: How to resolve the collision
    float penetrationDepth;          // Scalar length of overlap

    bool isTrigger; // If either collider is a trigger, no resolution
};