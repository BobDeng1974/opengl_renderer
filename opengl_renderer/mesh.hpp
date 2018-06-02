#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "primitives.hpp"
#include "shader.hpp"


namespace opengl {


class Mesh {
public:
    Vertices vertices;
    Indices indices;
    Textures textures;

    GLuint vao;

    Mesh(Vertices vertices, Indices indices, Textures textures);

    void render(const Shader& shader) const;

private:
    GLuint vbo, ibo;
};


using Meshes = std::vector<Mesh>;

} // namespace opengl