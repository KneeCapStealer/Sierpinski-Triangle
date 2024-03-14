#include "triangle.hpp"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>


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

std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, uint8_t depth)
{
    if (depth <= 1)
        return std::vector<Vertex>{input.begin(), input.end()};

    // Generate the upsidedown inverted triangle
    std::array<std::array<Vertex, 3>, 3> subTriangles = SubdivideTriangle(input);

    std::vector<Vertex> verticies{};
    for (const std::array<Vertex, 3>& subTriangle : subTriangles)
    {
        std::vector<Vertex> newSubTriangles;
        if (ShouldSkipTriangle(subTriangle))
        {
            // If the triangle is larger than the screen it should be checked further
            if (subTriangle[0].y - subTriangle[1].y > 2)
            {
                // Subdivide the large triangle.
                // If the subdivided triangles now are inside the screen they will be rendered
                // If they are outside they will simply be skipped and nothing happens
                for (std::array<Vertex, 3>& subSubTriangle : SubdivideTriangle(subTriangle))
                {
                    std::vector<Vertex> subDivision = SierpinskiTriangle(subSubTriangle, depth - 2);
                    newSubTriangles.insert(newSubTriangles.end(), subDivision.begin(), subDivision.end());
                }
            }
            else
                continue; 
        }
        else
        {
            newSubTriangles = SierpinskiTriangle(subTriangle, depth - 1);
        }

        verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
    }

    return verticies;
}
