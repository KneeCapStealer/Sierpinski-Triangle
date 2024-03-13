#include <cassert>
#include <glad/glad.h>

#include "shaders.hpp"
#include <iostream>
#include <malloc.h>

static uint32_t CompileShader(const uint32_t type, const std::string& source)
{
    uint32_t id = glCreateShader(type);
    const char* sourceCstr = source.c_str();
    glShaderSource(id, 1, &sourceCstr, nullptr);
    glCompileShader(id);

    int32_t result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int32_t len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        std::string message;
        message.resize(len);

        glGetShaderInfoLog(id, len, &len, &message[0]);
        std::cerr << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader\n"
                  << message
                  << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    uint32_t program = glCreateProgram();
    uint32_t vert = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    assert(vert && "VertexShader failed to compile");
    assert(frag && "FragmentShader failed to compile");

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    // glDetachShader(program, vert);
    // glDetachShader(program, frag);

    return program;
}
