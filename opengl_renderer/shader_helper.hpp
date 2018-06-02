#pragma once

#include <GL/glew.h>

#include <fstream>
#include <streambuf>
#include <string>


namespace opengl {


template <GLuint Type>
GLuint compile_shader(const std::string& source);

template <GLuint Type, class Shader>
GLuint create_shader(Shader&& path);


template <GLuint... Types, class... Shaders>
GLuint compile_shaders(Shaders&&... shader_paths) {
    auto program = glCreateProgram();

    auto shaders = { create_shader<Types>(std::forward<Shaders>(shader_paths))... };

    for (auto& shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    for (auto& shader : shaders) {
        glDeleteShader(shader);
    }

    return program;
}


template <GLuint Type, class Shader>
GLuint create_shader(Shader&& path) {
    std::ifstream file(std::forward<Shader>(path));
    std::string shader;

    file.seekg(0, std::ios::end);
    shader.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    shader.assign((
        std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    auto id = compile_shader<Type>(shader);

    return id;
}


template <GLuint Type>
GLuint compile_shader(const std::string& source) {
    auto id = glCreateShader(Type);
    auto src = (const GLchar*)source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    return id;
}

} // namespace opengl