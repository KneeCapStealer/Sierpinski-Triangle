#include "triangle.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include <iostream>

std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> GenerateSierpinskyTriangle(
    const float positions[6],
    const uint32_t indices[3],
    uint32_t curMaxIndex,
    uint8_t depth)
{
    if (depth == 0) {
        std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> out;
        out.reset();
        return out;
    }

    Vertex A(positions[0], positions[1]);
    const uint32_t AIndex = indices[0];

    Vertex B(positions[2], positions[3]);
    const uint32_t BIndex = indices[1];

    Vertex C(positions[4], positions[5]);
    const uint32_t CIndex = indices[2];

    Vertex AB = A + ((B - A) / 2.f);
    const uint32_t ABIndex = ++curMaxIndex;
    
    Vertex AC = A + ((C - A) / 2.f);
    const uint32_t ACIndex = ++curMaxIndex;
    
    Vertex BC = B + ((C - B) / 2.f);
    const uint32_t BCIndex = ++curMaxIndex;

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

    auto outPair = std::make_pair(std::vector<float>(), std::vector<uint32_t>());
    outPair.first.insert(outPair.first.end(), {AB.x, AB.y, AC.x, AC.y, BC.x, BC.y});

    for (uint8_t i = 0; i < 3; i++)
    {
        outPair.second.insert(outPair.second.end(), &triangleIndices[i][0], &triangleIndices[i][2]);

        auto pair = GenerateSierpinskyTriangle(
            reinterpret_cast<float*>(triangles[i].data()),
            triangleIndices[i],
            curMaxIndex,
            depth - 1
        );

        if (!pair.has_value())
        {
            std::cout << "it stopped" << std::endl;
            continue;
        }

        std::cout << "guh" << std::endl;

        outPair.first.resize(outPair.first.size() + pair->first.size());
        outPair.first.insert(outPair.first.end(), pair->first.begin(), pair->first.end());

        outPair.second.resize(outPair.second.size() + pair->second.size());
        outPair.second.insert(outPair.second.end(), pair->second.begin(), pair->second.end());
    }

    return std::make_optional(outPair);
}

std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, uint8_t depth)
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
    for (const std::array<Vertex, 3>& triangle: subTriangles)
    {
        auto newSubTriangles = SierpinskiTriangle(triangle, depth - 1);
        verticies.insert(std::end(verticies), std::begin(newSubTriangles), std::end(newSubTriangles));
    }

    return verticies;
}
