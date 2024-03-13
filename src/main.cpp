// std
#include <cstdint>
#include <iostream>
#include <cmath>

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

    GLFWwindow* window = glfwCreateWindow(1000, 750, "Hello World!", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

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

    float positions[6] = {
         0.0f,  0.5f,
        -0.5f, -0.5f,
         0.5f, -0.5f
    };
    uint32_t indices[3] = {0, 1, 2};

    std::cout << glGetString(GL_VERSION) << std::endl;

    constexpr uint32_t depth = 1;
    // TODO: Fix wrong amount of vertices.
    auto output = GenerateSierpinskyTriangle(positions, indices, 2, depth).value();

    std::cout << "Vertex count: " << output.first.size()
              << "\n Index count: " << output.second.size()
              << std::endl;

    // Vertex Buffer Object
    uint32_t VBO;
    glCreateBuffers(1, &VBO);
    // glNamedBufferStorage(VBO, sizeof(float) * 6, positions, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(
        VBO,
        sizeof(float) * output.first.size(),
        output.first.data(),
        GL_DYNAMIC_STORAGE_BIT
    );

    uint32_t EBO;
    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(
        EBO,
        sizeof(uint32_t) * output.second.size(),
        output.second.data(),
        GL_DYNAMIC_STORAGE_BIT
    );


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

    glVertexArrayElementBuffer(VAO, EBO);

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
        "    color = vec4(1.0, 0.0, 0.1, 1.0);\n"
        "}\n";

    uint32_t shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glBindVertexArray(VAO);
    glClearColor(0.1f, 0.1f, 0.17f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(
            GL_TRIANGLES,
            output.second.size(),
            GL_UNSIGNED_INT, NULL
        );

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
