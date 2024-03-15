#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <optional>
#include <array>
#include <cmath>

struct Vertex
{
    float x;
    float y;

    constexpr Vertex(float x, float y): x{x}, y{y} {}

    constexpr Vertex operator+ (const Vertex& other) const { return Vertex(x + other.x, y + other.y); }
    constexpr Vertex operator- (const Vertex& other) const { return Vertex(x - other.x, y - other.y); }
    constexpr Vertex operator/ (const float val) const { return Vertex(x / val, y / val); }
    template<typename T>
    constexpr Vertex operator* (const T val) const { return Vertex(x * val, y * val); }

    constexpr float Length() const
    {
        return std::sqrt(x*x + y*y);
    }

    constexpr Vertex Normalize() const
    {
        return Vertex(x, y) / Length();
    }
    
    Vertex Lerp(const Vertex& other, float t) const
    {
        float lerpX = std::lerp(x, other.x, t);
        float lerpY = std::lerp(y, other.y, t);

        return Vertex(lerpX, lerpY);
    }

    bool IsInside(const float xMin, const float xMax, const float yMin, const float yMax) const
    {
        return x >= xMin &&
               x <= xMax &&
               y >= yMin &&
               y <= yMax;
    }
};

// std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, uint8_t depth);
std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, float minHeight);


constexpr uint32_t CustomPow(uint32_t base, uint32_t exp)
{
    uint32_t result = 1;
    while (true)
    {
        if (exp & 1)
            result *= base;

        exp >>= 1;
        if (!exp)
            break;

        base *= base;
    }

    return result;
}

bool ShouldSkipTriangle(const std::array<Vertex, 3>& triangle)
{
    // If all the verticies are outside the screen 
    return !triangle[0].IsInside(-1.f, 1.f, -1.f, 1.f) && 
           !triangle[1].IsInside(-1.f, 1.f, -1.f, 1.f) &&
           !triangle[2].IsInside(-1.f, 1.f, -1.f, 1.f);

}

std::array<std::array<Vertex, 3>, 3> SubdivideTriangle(const std::array<Vertex, 3>& triangle)
{
    std::array<Vertex, 3> upsideTriangle{{
        (triangle[0] + triangle[2]) / 2.f,
        (triangle[0] + triangle[1]) / 2.f,
        (triangle[1] + triangle[2]) / 2.f
    }};

    std::array<std::array<Vertex, 3>, 3> subTriangles{{
        // Triangle 1
        {{
            triangle[0],
            upsideTriangle[1],
            upsideTriangle[0]
        }},
        // Triangle 2
        {{
            upsideTriangle[1],
            triangle[1],
            upsideTriangle[2]
        }},
        // Triangle 3
        {{
            upsideTriangle[0],
            upsideTriangle[2],
            triangle[2]
        }}

    }};
    return subTriangles;
}