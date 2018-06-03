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
    std::array<std::unique_ptr<opengl::Model>, 2> model;
    std::array<std::unique_ptr<glm::mat4>, 2> model_mat;
    std::array<std::unique_ptr<glm::mat3>, 2> model_mat_it;

    std::shared_ptr<opengl::Shader> shader;
    GLuint mat_location = 0, mat_it_location = 0;

    const opengl::Model* current_model() const noexcept;

    void swap();

private:
    int current = 0;
};

} // namespace opengl::scenes