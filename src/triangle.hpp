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

std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> GenerateSierpinskyTriangle(const float positions[6], const uint32_t indices[3], uint32_t curMaxIndex, uint8_t depth);

std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, uint8_t depth);

template<uint32_t size>
std::pair<std::array<Vertex, size>, std::array<uint32_t, size>> SierpinskiTriangleIndices(
    const std::array<Vertex, 3>& input,
    uint8_t depth)
{
    if (depth <= 1)
        return std::vector<Vertex>{input.begin(), input.end()};

    // Generate the upsidedown inverted triangle
    std::array<Vertex, 3> upsideTriangle{{
        (input[0] + input[2]) / 2.f,
        (input[0] + input[1]) / 2.f,
        (input[1] + input[2]) / 2.f
    }};

    std::array<std::array<Vertex, 3>, 3> subTriangles{{
        // Triangle 1
        {{
            input[0],
            upsideTriangle[1],
            upsideTriangle[0]
        }},
        // Triangle 2
        {{
            upsideTriangle[1],
            input[1],
            upsideTriangle[2]
        }},
        // Triangle 3
        {{
            upsideTriangle[0],
            upsideTriangle[2],
            input[2]
        }}
    }};

    std::vector<Vertex> verticies{};
    for (const std::array<Vertex, 3>& triangle : subTriangles)
    {
        auto newSubTriangles = SierpinskiTriangle(triangle, depth - 1);
        verticies.insert(std::end(verticies), std::begin(newSubTriangles), std::end(newSubTriangles));
    }

    return verticies;
}

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
