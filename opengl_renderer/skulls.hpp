#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <memory>

#include "model.hpp"
#include "shader.hpp"


namespace opengl::scenes {


class Skulls {
public:
    std::array<std::shared_ptr<opengl::Model>, 2> model;
    std::array<std::shared_ptr<glm::mat4>, 2> model_mat;
    std::array<std::shared_ptr<glm::mat3>, 2> model_mat_it;

    std::shared_ptr<opengl::Shader> shader;
    GLuint mat_location = 0, mat_it_location = 0;

    std::shared_ptr<opengl::Model> current_model() const;

    void swap();

private:
    int current = 0;
};

} // namespace opengl::scenes