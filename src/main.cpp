// std
#include <cstdint>
#include <iostream>
#include <cmath>
#include <chrono>
#include <ranges>

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// own stuff
#include "shaders.hpp"
#include "triangle.hpp"

void processInput(GLFWwindow *window);

int main(void) {
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World!", glfwGetPrimaryMonitor(), NULL);

    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    if ( !GLAD_GL_ARB_direct_state_access )
    {
       /* see
       * https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_direct_state_access.txt
       * https://www.khronos.org/opengl/wiki/Direct_State_Access. This is the way.
       */
      std::cout << "DSA not supported!" << std::endl;
      return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // TODO: Fix wrong amount of vertices.
    // positions.reserve(positions.size() + output.first.size());
    // positions.insert(positions.end(), output.first.begin(), output.first.end());

    // std::vector<uint32_t> indices = output.second;

    // std::vector<uint32_t> indices({0, 1, 2});

    // for (int i = 0; i < output.size(); i++)
    // {
    //     std::cout << "x: " << output[i].x << ", y: " << output[i + 1].y << '\n';
    // }

    // Vertex Buffer Object
    uint32_t VBO;
    glCreateBuffers(1, &VBO);
    // glNamedBufferStorage(VBO, sizeof(float) * 6, positions, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(
        VBO,
        NULL,
        NULL,
        GL_DYNAMIC_STORAGE_BIT
    );
    // uint32_t EBO;
    // glCreateBuffers(1, &EBO);
    // glNamedBufferStorage(
    //     EBO,
    //     sizeof(uint32_t) * indices.size(),
    //     indices.data(),
    //     GL_DYNAMIC_STORAGE_BIT
    // );


    // Vertex Array Object
    uint32_t VAO;
    glCreateVertexArrays(1, &VAO);
    glVertexArrayVertexBuffer(
        VAO,
        0,
        VBO,
        0,
        2 * sizeof(float)
    );

    // glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, false, 0);

    glVertexArrayAttribBinding(VAO, 0, 0);

    std::string vertexShader = 
        "#version 330 core\n"
        "\n"
        "layout(location=0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"
        "}\n";

    std::string fragmentShader = 
        "#version 330 core\n"
        "\n"
        "layout(location=0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    color = vec4(1.0, 0.3, 0.1, 1.0);\n"
        "}\n";

    uint32_t shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glBindVertexArray(VAO);
    glClearColor(0.07f, 0.04f, 0.12f, 1.0f);
    glEnable(GL_MULTISAMPLE);


    auto start = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        static int64_t time = 0;
        static float scale = 0;
        auto end = std::chrono::high_resolution_clock::now();

        const int64_t deltatime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); 
        time += deltatime;

        start = std::chrono::high_resolution_clock::now();

        // Default triangle values
        static std::vector<Triangle> largeTriangles{{{
            { 0.f,  1.f},
            {-1.f, -1.f},
            { 1.f, -1.f}
        }}};
        std::cout << largeTriangles.size() << std::endl;
        exit(0);

        // t is time but in seconds
        const float t = (float)time / 1000000.f;
        const float newScale = (float)std::pow(2, t);
        const float scaleDiff = scale - newScale;
        scale = newScale;

        // Point to zoom at
        constexpr Vertex zoomPoint{0.f, 1.f};
        
        // A c++ range to scale all the elements of the largeTriangles
        // Removes triangles that might have become invisible
        auto scaledIterator = largeTriangles
                                | std::views::transform([=] (const Triangle& tri)
                                                        { return ScaleTriangle(tri, scale, zoomPoint); })
                                | std::views::filter([=] (const Triangle& tri)
                                                     { return !TriangleIsVisible(tri); });

        // Copy the newly scaled values into 
        largeTriangles.clear();
        largeTriangles.insert_range(largeTriangles.begin(), scaledIterator);

        // Check if smaller triangles should replace the current triangles
        for (auto triangle = largeTriangles.begin(); triangle != largeTriangles.end();)
        {
            // Check it's subTriangles are all visible
            bool allTrisVisible = true;
            std::array<Triangle, 3> subTriangles = SubdivideTriangle(*triangle);
            for (Triangle& subTriangle : subTriangles)
            {
                if (!TriangleIsVisible(subTriangle))
                {
                    allTrisVisible = false;
                    break;
                }
            }

            // If some of it's subTriangles arent visible add them all to the vector
            // The invisible ones will be removed later in the loop
            // Remove the current big triangle and don't increment
            if (!allTrisVisible)
            {
                largeTriangles.erase(triangle);
                largeTriangles.insert_range(largeTriangles.end(), subTriangles);
                continue;
            }

            // If this triangle can't be split into smaller parts and is visible
            // Increment the iterator and go to the next index
            triangle++;
        }

        // std::cout << "Collided!!!!\n"
        //           << "New points are:\n"
        //           << "[ x: " << largeTriangles[0].x
        //           << ", y: " << largeTriangles[0].y << " ],\n"
        //           << "[ x: " << largeTriangles[1].x
        //           << ", y: " << largeTriangles[1].y << " ],\n"
        //           << "[ x: " << largeTriangles[2].x
        //           << ", y: " << largeTriangles[2].y << " ],\n\n";

        std::vector<Vertex> output;
        for (Triangle& triangle : largeTriangles)
            output.insert_range(output.end(), SierpinskiTriangle(triangle, 0.005f));

        std::cout << "FPS:\t" << 1.f / ((float)deltatime / 1000000.f) << '\n'
                  << "size:\t" << output.size() << '\n';
                  // << "Scale:\t" << scale << '\n';

        glNamedBufferData(
            VBO,
            (GLsizeiptr)output.size() * (GLsizeiptr)sizeof(Vertex),
            output.data(),
            GL_STATIC_DRAW
        );

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)output.size());
        // glDrawElements(
        //     GL_TRIANGLES,
        //     indices.size(),
        //     GL_UNSIGNED_INT,
        //     NULL
        // );

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
