#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>


namespace opengl {


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};


enum class texture_type {
    diffuse,
    specular
};

struct Texture {
    GLuint id;
    texture_type type;
    std::string path;
};


using Vertices = std::vector<Vertex>;
using Indices = std::vector<GLuint>;
using Textures = std::vector<Texture>;

} // namespace opengl