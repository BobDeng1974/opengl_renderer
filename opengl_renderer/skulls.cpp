#include "skulls.hpp"


namespace opengl::scenes {


const opengl::Model* Skulls::current_model() const noexcept {
    return model[current].get();
}

void Skulls::swap() {
    current ^= 1;

    shader->use();
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, glm::value_ptr(*model_mat[current]));
    glUniformMatrix3fv(mat_it_location, 1, GL_FALSE, glm::value_ptr(*model_mat_it[current]));
}

} // namespace opengl::scenes