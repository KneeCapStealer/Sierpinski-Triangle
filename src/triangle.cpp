#include "triangle.hpp"
#include <cstdint>
#include <optional>
#include <utility>

struct Vertex
{
    float x;
    float y;

    Vertex(float x, float y): x{x}, y{y} {}

    Vertex operator+ (const Vertex& other) const { return Vertex(x + other.x, y + other.y); }
    Vertex operator- (const Vertex& other) const { return Vertex(x - other.x, y - other.y); }
    Vertex operator/ (const float val) const { return Vertex(x / val, y / val); }
};

std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> GenerateSierpinskyTriangle(const float positions[6], const uint32_t indices[3], uint32_t curMaxIndex, uint8_t depth)
{
    if (depth == 0) {
        std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> out;
        return out;
    }

    Vertex A(positions[0], positions[1]);
    const uint32_t AIndex = indices[0];

    Vertex B(positions[2], positions[3]);
    const uint32_t BIndex = indices[1];

    Vertex C(positions[4], positions[5]);
    const uint32_t CIndex = indices[2];

    Vertex AB = (A + (B - A) / 2.f);
    const uint32_t ABIndex = curMaxIndex + 1;
    
    Vertex AC = (A + (C - A) / 2.f);
    const uint32_t ACIndex = curMaxIndex + 2;
    
    Vertex BC = (B + (C - B) / 2.f);
    const uint32_t BCIndex = curMaxIndex + 3;

    std::vector<Vertex> triangles[3] = {
        {A, AB, AC},
        {AB, B, BC},
        {AC, BC, C}
    };

    uint32_t triangleIndices[3][3] = {
        {AIndex, ABIndex, ACIndex},
        {ABIndex, BIndex, BCIndex},
        {ACIndex, BCIndex, CIndex}
    };

    auto outPair = std::make_pair(std::vector<float>(6), std::vector<uint32_t>(3));
    outPair.first.insert(outPair.first.end(), {AB.x, AB.y, AC.x, AC.y, BC.x, BC.y});

    for (uint8_t i = 0; i < 3; i++)
    {
        outPair.second.insert(outPair.second.end(), &triangleIndices[i][0], &triangleIndices[i][2]);

        auto pair = GenerateSierpinskyTriangle(
            reinterpret_cast<float*>(triangles[i].data()),
            triangleIndices[i],
            curMaxIndex + outPair.first.size(),
            depth - 1
        );

        if (!pair.has_value())
            continue;

        outPair.first.insert(outPair.first.end(), pair->first.begin(), pair->first.end());
        outPair.second.insert(outPair.second.end(), pair->second.begin(), pair->second.end());
    }

    return std::make_optional(outPair);
}
