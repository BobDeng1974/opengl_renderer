#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <vector>


namespace opengl {


template <std::size_t Count>
struct Lighting {
    struct Light {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::array<Light, Count> lights;
    GLfloat ambient;
    GLfloat diffuse;
    GLfloat specular;

};

} // namespace opengl