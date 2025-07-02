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

    Vec2& operator-=(const Vec2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2{ x - other.x, y - other.y };
    }

    Vec2 operator-() const {
        return Vec2{ -x, -y };
    }

    Vec2 operator*(float scalar) const {
        return Vec2{ x * scalar, y * scalar };
    }

    

    Vec2 operator/(float scalar) const {
        return Vec2{ x / scalar, y / scalar };
    }



    float length() const {
        return std::sqrt(x * x + y * y);
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    Vec2 clamp(const Vec2& min, const Vec2& max) const {
        return {
            (x < min.x) ? min.x : (x > max.x) ? max.x : x,
            (y < min.y) ? min.y : (y > max.y) ? max.y : y
        };
    }

    Vec2 normalized() const {
        float len = length();
        return len > 0 ? Vec2{ x / len, y / len } : Vec2{ 0, 0 };
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }

    static Vec2 UnitVec() {
        return Vec2{ 1.f, 1.f };
    }

    static Vec2 ZeroVec() {
        return Vec2{ 0.f, 0.f };
    }

    static float Dot(Vec2 a, Vec2 b) {
        return a.x * b.x + a.y * b.y;
    }
};

inline Vec2 operator*(float scalar, const Vec2& vec) {
    return Vec2{ vec.x * scalar, vec.y * scalar };
}

