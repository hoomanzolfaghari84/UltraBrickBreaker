#pragma once
#include <cmath>
#include <ostream>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const {
        return Vec2{ x + other.x, y + other.y };
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x; y += other.y;
        return *this;
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2{ x - other.x, y - other.y };
    }

    Vec2 operator*(float scalar) const {
        return Vec2{ x * scalar, y * scalar };
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalized() const {
        float len = length();
        return len > 0 ? Vec2{ x / len, y / len } : Vec2{ 0, 0 };
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};
