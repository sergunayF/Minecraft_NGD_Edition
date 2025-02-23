#pragma once

#include "raylib.h"
#include <functional>
#include <cmath>

struct Vector3Hash {
    size_t operator()(const Vector3& v) const {
        return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^ (std::hash<float>()(v.z) << 2);
    }
};

inline bool operator==(const Vector3& a, const Vector3& b) {
    return (fabs(a.x - b.x) < 0.001f) &&
        (fabs(a.y - b.y) < 0.001f) &&
        (fabs(a.z - b.z) < 0.001f);
}

struct Vector2Hash {
    size_t operator()(const Vector2& v) const {
        size_t hx = std::hash<float>{}(v.x);
        size_t hy = std::hash<float>{}(v.y);
        return hx ^ (hy << 1);
    }
};


inline bool operator==(const Vector2& a, const Vector2& b) {
    constexpr float epsilon = 0.0001f;
    return (std::fabs(a.x - b.x) < epsilon) &&
        (std::fabs(a.y - b.y) < epsilon);
}
