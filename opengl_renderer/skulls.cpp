#include "skulls.hpp"


namespace opengl::scenes {


std::shared_ptr<opengl::Model> Skulls::current_model() const noexcept {
    return model[current];
}

void Skulls::swap() {
    current ^= 1;

    shader->use();
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, glm::value_ptr(*model_mat[current]));
    glUniformMatrix3fv(mat_it_location, 1, GL_FALSE, glm::value_ptr(*model_mat_it[current]));
}

} // namespace opengl::scenes