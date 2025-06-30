#pragma once
#include "Utils/Vec2.hpp"
#include <SFML/Graphics/Color.hpp>

// Spatial data
struct Transform {
    Vec2 position;
    Vec2 size;
    float rotation = 0.0f;
};

// Movement
struct Velocity {
    Vec2 velocity;
};

// AABB Collider
struct AABB {
    Vec2 halfSize; // Use for collision
};

// Rendering
struct Renderable {
    sf::Color color = sf::Color::White;
    int zIndex = 0; // Optional layering
};

// Generic Tag component (per entity)
struct Tag {
    std::string name;
};
