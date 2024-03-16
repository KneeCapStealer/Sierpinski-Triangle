#include "triangle.hpp"
#include <array>


std::vector<Vertex> SierpinskiTriangle(const Triangle& input, float minHeight)
{
    if (input[0].y - input[1].y < minHeight)
        return std::vector<Vertex>{input.begin(), input.end()};

    // Generate the upsidedown inverted triangle
    std::array<Triangle, 3> subTriangles = SubdivideTriangle(input);

    std::vector<Vertex> verticies{};
    for (const Triangle& subTriangle : subTriangles)
    {
        if (!TriangleIsVisible(subTriangle))
            continue;

        std::vector<Vertex> newSubTriangles = SierpinskiTriangle(subTriangle, minHeight);
        verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
    }

    return verticies;
}

std::array<Triangle, 3> SubdivideTriangle(const Triangle& triangle)
{
    Triangle upsideTriangle{{
        (triangle[0] + triangle[2]) / 2.f,
        (triangle[0] + triangle[1]) / 2.f,
        (triangle[1] + triangle[2]) / 2.f
    }};

    std::array<Triangle, 3> subTriangles{{
        // Triangle 1
        {
            triangle[0],
            upsideTriangle[1],
            upsideTriangle[0]
        },
        // Triangle 2
        {
            upsideTriangle[1],
            triangle[1],
            upsideTriangle[2]
        },
        // Triangle 3
        {
            upsideTriangle[0],
            upsideTriangle[2],
            triangle[2]
        }
    }};

    return subTriangles;
}
