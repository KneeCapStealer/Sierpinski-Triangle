// std
#include <cstdint>
#include <iostream>
#include <cmath>
#include <chrono>

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// own stuff
#include "shaders.hpp"
#include "triangle.hpp"

void processInput(GLFWwindow *window);

int main(void) {
    std::cout << "Hello there" << std::endl;

    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW" << std::endl;
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
        auto end = std::chrono::high_resolution_clock::now();

        const int64_t diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); 
        time += diff;

        start = std::chrono::high_resolution_clock::now();

        static std::array<Vertex, 3> positions{{
            { 0.f,  1.f},
            {-1.f, -1.f},
            { 1.f, -1.f}
        }};
        
        const float t = time / 1000000.f;
        const float scale = std::pow(2, t);

        constexpr Vertex zoomPoint{0.f, 1.f};

        
        std::array<Vertex, 3> finalPositions{{
            {positions[0] * scale + zoomPoint * (1 -scale)},
            {positions[1] * scale + zoomPoint * (1 -scale)},
            {positions[2] * scale + zoomPoint * (1 -scale)},
        }};

        for (auto& subTriangle : SubdivideTriangle(finalPositions))
        {
            const bool bottumUnderScreen = subTriangle[1].y < -1.f &&
                                           subTriangle[2].y < -1.f;
        }
        
        std::vector<Vertex> output = SierpinskiTriangle(finalPositions, 0.01f);

        std::cout << "FPS:\t" << 1.f / (diff / 1000000.f) << '\n';

        glNamedBufferData(
            VBO,
            output.size() * sizeof(Vertex),
            output.data(),
            GL_STATIC_DRAW
        );

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, output.size());
        // glDrawElements(
        //     GL_TRIANGLES,
        //     indices.size(),
        //     GL_UNSIGNED_INT, NULL
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
