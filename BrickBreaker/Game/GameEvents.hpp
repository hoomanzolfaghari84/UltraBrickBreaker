#pragma once

#include "../Utils/Vec2.hpp"
#include "../Core/EventBus.hpp"
#include "../Core/ECS.hpp"


struct BallHitBrickEvent : public IEvent { Entity ball, brick; };
struct BallHitWallEvent : public IEvent { Entity ball; };
struct BallOutOfBoundsEvent : public IEvent { Entity ball; };