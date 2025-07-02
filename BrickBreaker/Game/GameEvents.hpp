#pragma once

#include "../Utils/Vec2.hpp"
#include "../Core/EventBus.hpp"
#include "../Core/ECS.hpp"


// Emitted when ball hits a wall (left/right or top)
struct BallHitWallEvent {
    enum class Side { LeftRight, Top };
    Entity ball;
    Side side;
};

// Emitted when ball hits paddle
struct BallHitPaddleEvent {
    Entity ball;
    Entity paddle;
};

// Emitted when ball hits brick
struct BallHitBrickEvent {
    Entity ball;
    Entity brick;
};

// Emitted when ball falls below bottom of screen
struct BallOutOfBoundsEvent {
    Entity ball;
};

//
struct ScoreChangedEvent {
    int delta;  // e.g., +100 for a brick
};
