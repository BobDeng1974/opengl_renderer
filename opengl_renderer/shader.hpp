#pragma once

#include <GL/glew.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "shader_helper.hpp"


namespace opengl {


class Shader {
public:
    Shader(GLuint program);
    virtual ~Shader();


    template <GLuint... Types, class... Shaders>
    static std::shared_ptr<Shader> create(const Shaders&... shader_paths) {
        GLuint program = compile_shaders<Types...>(shader_paths...);

        return std::make_shared<Shader>(program);
    }


    void use() const;
    GLuint uniform(const std::string& name) const;

    const GLuint program;

private:
    mutable std::unordered_map<std::string, GLuint> uniforms;
};

} // namespace opengl