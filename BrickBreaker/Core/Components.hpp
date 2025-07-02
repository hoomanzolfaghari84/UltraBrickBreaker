#pragma once
#include "../Utils/Vec2.hpp"
#include <SFML/Graphics/Color.hpp>
#include "../Utils/Constants.hpp"

struct Rect
{
    float x=0, y=0, w=1, h=1;
    

    Rect() = default;

    Rect(float x, float y, float w, float h)
        : x(x), y(y), w(w), h(h)
    {
    }
};


// Spatial data
struct Transform {
    Vec2 translation{0.f, 0.f};
    Vec2 scale{ 1.f, 1.f };;
    float rotation = 0.0f;

    Transform() = default;

    Transform(Vec2 translation, Vec2 scale, float rotation) : translation(translation), scale(scale), rotation(rotation)
    {

    }
};



// Generic Tag component (per entity)
struct Tag {
    std::string name;
};


/////////////
// Rendering

struct Color {
    int r = 255, g = 255, b = 255, a = 255;

    Color() = default;

    Color(int r, int g, int b, int a)
        : r(r), g(g), b(b), a(a)
    {
    }
};

struct Renderable {
    bool isVisible = true;
    int zIndex = 0; // Optional layering

    Renderable() = default;
};

struct RenderShape : Renderable {
    Color color = Color(255,255,255,255);

    RenderShape() = default;
};

struct RenderRectangle : public RenderShape
{
    Vec2 position = Vec2::ZeroVec(), size = Vec2::UnitVec();
    float rotation = 0;
};

struct RenderCircle : public RenderShape
{
    Vec2 position = Vec2::ZeroVec();
    float radius = 1.f;
};



struct CameraComponent {
    Entity target = -1;     // Follow target (e.g., player position)
    float zoom = 1.0f;
    bool isActive = true;
};


struct AnimationComponent {
    int currentFrame;
    int frameCount;
    float frameDuration;
    float timeSinceLastFrame;
    bool looping;
};






/////////////////////////////////
//Physics & Collision Components:

// Movement
struct Kinematics
{
    Vec2 position = {0.f, 0.f}, velocity = { 0.f, 0.f }, acceleration = { 0.f, 0.f };
    bool isStatic = false;

    void SetMass(float m) {
        mass = m;
        recalcInvMass();
    }

    Kinematics() = default;

    Kinematics(const Vec2& position, const Vec2& velocity, const Vec2& acceleration, bool isStatic, float mass)
        : position(position), velocity(velocity), acceleration(acceleration), isStatic(isStatic)
    {
        SetMass(mass);
    }

private:
    float mass = 1.0f;
    float invMass = 1.0f;

    void recalcInvMass() {
        invMass = isStatic || mass < EPSILON ? 0.0f : 1.0f / mass;
    }

    
};


enum class ColliderShapeType { Circle, Rectangle, VertexArray };

struct CircleCollisionShape {
    float radius = 1.0f;
};

struct RectangleCollisionShape {
    Vec2 halfsize = { 0.5f, 0.5f };
    float rotation = 0.f;
};

struct VertexArrayCollisionShape {
    std::vector<Vec2> vertices;
};

// Collider Component
struct Collider {
    ColliderShapeType type = ColliderShapeType::VertexArray;
    bool isTrigger = false;

    Vec2 position = { 0.f, 0.f };

    // Actual shape definition
    std::variant<CircleCollisionShape, RectangleCollisionShape, VertexArrayCollisionShape> shape;

};
