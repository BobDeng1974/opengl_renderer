#include "shader.hpp"


namespace opengl {


Shader::Shader(GLuint program) : program(program) {}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::use() const {
    glUseProgram(program);
}

GLuint Shader::uniform(const std::string & name) const {
    if (uniforms.find(name) == uniforms.end()) {
        uniforms[name] = glGetUniformLocation(program, name.c_str());
    }

    return uniforms[name];
}

} // namespace opengl