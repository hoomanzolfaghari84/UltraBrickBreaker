#pragma once
#include "Utils/Vec2.hpp"

struct CollisionEvent : public IEvent {
    Entity entityA;
    Entity entityB;
};

struct BoxOutOfBoundsEvent : IEvent {
    Entity entity;
    const Vec2 position;
    BoxOutOfBoundsEvent(Entity e, const Vec2 pos) : entity(e), position(pos) {};
};