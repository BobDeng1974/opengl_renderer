#include "mesh.hpp"

#include <utility>


namespace opengl {


Mesh::Mesh(Vertices&& vertices, Indices&& indices, Textures&& textures)
    : vertices{vertices},
      indices{indices},
      textures{textures}
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

void Mesh::render(const Shader& shader) const {
    GLint diffuse_n = 0;
    GLint specular_n = 0;

    for (std::size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);

        const std::string u_texture = [&] {
            switch (textures[i].type) {
            case texture_type::specular:
                return "u_texture_specular_" + std::to_string(specular_n++); break;
            case texture_type::diffuse: [[fallthrough]];
            default:
                return "u_texture_diffuse_" + std::to_string(diffuse_n++); break;
            }
        }();


        glUniform1i(shader.uniform(u_texture), (GLint)i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

} // namespace opengl