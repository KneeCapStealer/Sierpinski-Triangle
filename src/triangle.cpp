#include "triangle.hpp"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>



// std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, uint8_t depth)
// {
//     if (depth <= 1)
//         return std::vector<Vertex>{input.begin(), input.end()};

//     // Generate the upsidedown inverted triangle
//     std::array<std::array<Vertex, 3>, 3> subTriangles = SubdivideTriangle(input);

//     std::vector<Vertex> verticies{};
//     for (const std::array<Vertex, 3>& subTriangle : subTriangles)
//     {
//         if (!ShouldSkipTriangle(subTriangle))
//         {
//             std::vector<Vertex> newSubTriangles = SierpinskiTriangle(subTriangle, depth - 1);
//             verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
//             continue;
//         }

//         // If the triangle is larger than the screen it should be checked further
//         if (!(subTriangle[0].y - subTriangle[1].y > 2))
//             continue; 

//         // Subdivide the large triangle.
//         // If the subdivided triangles now are inside the screen they will be rendered
//         // If they are outside they will simply be skipped and nothing happens
//         for (std::array<Vertex, 3>& subSubTriangle : SubdivideTriangle(subTriangle))
//         {
//             std::vector<Vertex> newSubTriangles =  SierpinskiTriangle(subSubTriangle, depth - 2);
//             verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
//         }
//     }

//     return verticies;
// }


std::vector<Vertex> SierpinskiTriangle(const std::array<Vertex, 3>& input, float minHeight)
{
    if (input[0].y - input[1].y < minHeight)
        return std::vector<Vertex>{input.begin(), input.end()};

    // Generate the upsidedown inverted triangle
    std::array<std::array<Vertex, 3>, 3> subTriangles = SubdivideTriangle(input);

    std::vector<Vertex> verticies{};
    for (const std::array<Vertex, 3>& subTriangle : subTriangles)
    {
        if (!ShouldSkipTriangle(subTriangle))
        {
            std::vector<Vertex> newSubTriangles = SierpinskiTriangle(subTriangle, minHeight);
            verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
            continue;
        }

        // If the triangle is larger than the screen it should be checked further
        if (!(subTriangle[0].y - subTriangle[1].y > 2))
            continue; 

        // Subdivide the large triangle.
        // If the subdivided triangles now are inside the screen they will be rendered
        // If they are outside they will simply be skipped and nothing happens
        for (std::array<Vertex, 3>& subSubTriangle : SubdivideTriangle(subTriangle))
        {
            std::vector<Vertex> newSubTriangles =  SierpinskiTriangle(subSubTriangle, minHeight);
            verticies.insert(verticies.end(), newSubTriangles.begin(), newSubTriangles.end());
        }
    }

    return verticies;
}
