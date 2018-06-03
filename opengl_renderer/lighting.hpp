#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <string>
#include <vector>


namespace opengl {


template <std::size_t Count>
struct Lighting {
    struct Light {
        glm::vec3 position;
        glm::vec3 color;
        GLuint location;
    };

    struct Attenuation {
        GLfloat constant, linear, quadratic;
    };

    std::array<Light, Count> lights;
    Attenuation attenuation;

    GLfloat ambient;
    GLfloat diffuse;
    GLfloat specular;

    const std::size_t size = Count;
};

} // namespace opengl