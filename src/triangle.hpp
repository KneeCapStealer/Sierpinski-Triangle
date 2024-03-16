#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <cmath>

struct Vertex
{
    float x;
    float y;

    constexpr Vertex(float x, float y): x{x}, y{y} {}

    constexpr Vertex operator+ (const Vertex other) const { return Vertex(x + other.x, y + other.y); }
    constexpr Vertex operator- (const Vertex other) const { return Vertex(x - other.x, y - other.y); }
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
};

typedef std::array<Vertex, 3> Triangle;

std::array<Triangle, 3> SubdivideTriangle(const Triangle& triangle);

std::vector<Vertex> SierpinskiTriangle(const Triangle& input, float minHeight);


inline constexpr bool TriangleIsVisible(const Triangle& triangle)
{
    constexpr const auto IsBiggerY = [] (const Vertex a, const Vertex b) { return a.y > b.y; };
    constexpr const auto IsBiggerX = [] (const Vertex a, const Vertex b) { return a.x > b.x; };

    const float top = std::max_element(triangle.begin(), triangle.end(), IsBiggerY)->y;
    const float bottom = std::min_element(triangle.begin(), triangle.end(), IsBiggerY)->y;
    const float right = std::max_element(triangle.begin(), triangle.end(), IsBiggerX)->x;
    const float left = std::min_element(triangle.begin(), triangle.end(), IsBiggerX)->x;

    return top    > -1.f &&
           bottom <  1.f &&
           right  > -1.f &&
           left   <  1.f;
}


inline constexpr Triangle ScaleTriangle(
    const Triangle& triangle,
    const float scale,
    const Vertex zoomPoint)
{
    const float scaleDiff = (1 - scale);
    return {{
        { triangle[0] * scale + zoomPoint * scaleDiff },
        { triangle[1] * scale + zoomPoint * scaleDiff },
        { triangle[2] * scale + zoomPoint * scaleDiff },
    }};

}

inline constexpr uint32_t CustomPow(uint32_t base, uint32_t exp)
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

